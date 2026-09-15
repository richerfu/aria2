/* SPDX-License-Identifier: GPL-2.0-or-later WITH OpenSSL-exception */
#include <aria2/aria2_c.h>
#include <aria2/aria2.h>

#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <set>
#include <thread>

#include "LogFactory.h"
#include "Option.h"
#include "OptionHandler.h"
#include "OptionParser.h"
#include "Platform.h"
#include "prefs.h"

struct aria2_c_session {
  aria2::Session* engine;
  std::thread::id owner;
  bool stopped;
  bool failed;
};

namespace {
std::mutex ownerMutex;
aria2_c_session* activeSession = nullptr;
// Upstream documents libraryInit/libraryDeinit once per process. Never touch
// its freed preference globals after the lifecycle has been consumed.
bool libraryUsed = false;
constexpr size_t MAX_OPTIONS = 256;
constexpr size_t MAX_NAME = 128;
constexpr size_t MAX_VALUE = 65536;
constexpr size_t MAX_OPTION_BYTES = 1024 * 1024;

bool boundedString(const char* value, size_t limit, size_t& length)
{
  if (!value) {
    return false;
  }
  length = strnlen(value, limit + 1);
  return length <= limit;
}

int checkOwner(aria2_c_session* session)
{
  // Check identity before dereferencing a caller-provided opaque handle.
  if (!session || session != activeSession) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  if (session->owner != std::this_thread::get_id()) {
    return ARIA2_C_WRONG_THREAD;
  }
  return ARIA2_C_OK;
}

// Fails closed before upstream option_processing can print a parse diagnostic.
// The upstream library also initializes its console objects as discard sinks.
int copyOptions(const aria2_c_option* input, size_t count,
                aria2::KeyVals& output)
{
  if (count > MAX_OPTIONS || (count && !input)) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  std::set<std::string> names;
  size_t total = 0;
  bool rpc = false;
  bool secret = false;
  for (size_t i = 0; i < count; ++i) {
    size_t nameLength, valueLength;
    if (!boundedString(input[i].name, MAX_NAME, nameLength) || !nameLength ||
        !boundedString(input[i].value, MAX_VALUE, valueLength)) {
      return ARIA2_C_INVALID_ARGUMENT;
    }
    total += nameLength + valueLength;
    if (total > MAX_OPTION_BYTES || !names.insert(input[i].name).second) {
      return ARIA2_C_INVALID_ARGUMENT;
    }
    std::string name(input[i].name, nameLength);
    std::string value(input[i].value, valueLength);
    if (name == "conf-path" || name == "log" || name == "log-level" ||
        name == "console-log-level" || name == "quiet" || name == "daemon" ||
        name == "rpc-user" || name == "rpc-passwd" || name == "rpc-secure" ||
        name == "rpc-save-upload-metadata") {
      return ARIA2_C_INVALID_ARGUMENT;
    }
    if ((name == "no-conf" && value != "true") ||
        (name == "rpc-listen-all" && value != "false") ||
        (name == "rpc-allow-origin-all" && value != "false")) {
      return ARIA2_C_INVALID_ARGUMENT;
    }
    if (name == "enable-rpc") {
      rpc = value == "true";
    }
    if (name == "rpc-secret") {
      // Caller owns entropy. Restrict to printable token bytes, not whitespace.
      if (valueLength < 32 || valueLength > 1024) {
        return ARIA2_C_INVALID_ARGUMENT;
      }
      for (unsigned char c : value) {
        if (c < 0x21 || c > 0x7e) {
          return ARIA2_C_INVALID_ARGUMENT;
        }
      }
      secret = true;
    }
    output.emplace_back(std::move(name), std::move(value));
  }
  if (rpc && !secret) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  output.emplace_back("no-conf", "true");
  output.emplace_back("quiet", "true");
  output.emplace_back("log", "");
  output.emplace_back("rpc-listen-all", "false");
  output.emplace_back("rpc-allow-origin-all", "false");
  output.emplace_back("rpc-save-upload-metadata", "false");
  return ARIA2_C_OK;
}

struct LibraryGuard {
  bool initialized = false;
  ~LibraryGuard()
  {
    if (initialized) {
      try {
        aria2::libraryDeinit();
        // A throwing Platform constructor has no owner to destroy.
        if (aria2::Platform::isInitialized()) {
          aria2::Platform::tearDown();
        }
      }
      catch (...) {
        // Never unwind from cleanup across the C ABI.
      }
    }
  }
};
} // namespace

extern "C" uint32_t aria2_c_abi_version(void) { return 1; }

extern "C" int aria2_c_initialize(const aria2_c_option* options, size_t count,
                                  aria2_c_session** out)
{
  if (!out) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  *out = nullptr;
  bool consumed = false;
  try {
    std::unique_lock<std::mutex> lock(ownerMutex, std::try_to_lock);
    if (!lock.owns_lock() || activeSession) {
      return ARIA2_C_BUSY;
    }
    if (libraryUsed) {
      return ARIA2_C_PROCESS_RESTART_REQUIRED;
    }
    if (aria2::Platform::isInitialized()) {
      return ARIA2_C_BUSY;
    }
    aria2::KeyVals values;
    auto status = copyOptions(options, count, values);
    if (status != ARIA2_C_OK) {
      return status;
    }
    aria2::Option validation;
    const auto& parser = aria2::OptionParser::getInstance();
    for (const auto& value : values) {
      auto pref = aria2::option::k2p(value.first);
      if (!pref) {
        return ARIA2_C_INVALID_ARGUMENT;
      }
      const auto* handler = parser->find(pref);
      if (!handler) {
        return ARIA2_C_INVALID_ARGUMENT;
      }
      try {
        handler->parse(validation, value.second);
      }
      catch (...) {
        return ARIA2_C_INVALID_ARGUMENT;
      }
    }
    LibraryGuard guard;
    // Only validated input consumes the upstream process-global lifecycle.
    libraryUsed = true;
    consumed = true;
    guard.initialized = true;
    if (aria2::libraryInit() != 0) {
      return ARIA2_C_INIT_FAILED;
    }
    auto owner = std::unique_ptr<aria2_c_session>(
        new aria2_c_session{nullptr, std::this_thread::get_id(), false, false});
    aria2::SessionConfig config;
    config.keepRunning = true;
    config.useSignalHandler = false;
    owner->engine = aria2::sessionNew(values, config);
    if (!owner->engine) {
      return ARIA2_C_INIT_FAILED;
    }
    aria2::LogFactory::setConsoleOutput(false);
    activeSession = owner.release();
    guard.initialized = false;
    *out = activeSession;
    return ARIA2_C_OK;
  }
  catch (const std::bad_alloc&) {
    return consumed ? ARIA2_C_PROCESS_RESTART_REQUIRED : ARIA2_C_NO_MEMORY;
  }
  catch (...) {
    return consumed ? ARIA2_C_PROCESS_RESTART_REQUIRED
                    : ARIA2_C_INVALID_ARGUMENT;
  }
}

extern "C" int aria2_c_run_once(aria2_c_session* session, int* running)
{
  if (!running) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  *running = 0;
  try {
    std::unique_lock<std::mutex> lock(ownerMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
      return ARIA2_C_BUSY;
    }
    auto status = checkOwner(session);
    if (status != ARIA2_C_OK) {
      return status;
    }
    if (session->failed) {
      return ARIA2_C_ENGINE_ERROR;
    }
    if (!session->stopped) {
      try {
        int result = aria2::run(session->engine, aria2::RUN_ONCE);
        if (result < 0) {
          session->failed = true;
          return ARIA2_C_ENGINE_ERROR;
        }
        session->stopped = result == 0;
      }
      catch (...) {
        session->failed = true;
        return ARIA2_C_ENGINE_ERROR;
      }
    }
    *running = session->stopped ? 0 : 1;
    return ARIA2_C_OK;
  }
  catch (...) {
    return ARIA2_C_ENGINE_ERROR;
  }
}

extern "C" int aria2_c_shutdown(aria2_c_session* session, int force)
{
  if (force != 0 && force != 1) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  try {
    std::unique_lock<std::mutex> lock(ownerMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
      return ARIA2_C_BUSY;
    }
    auto status = checkOwner(session);
    if (status != ARIA2_C_OK) {
      return status;
    }
    if (session->failed) {
      return ARIA2_C_ENGINE_ERROR;
    }
    if (session->stopped) {
      return ARIA2_C_OK;
    }
    try {
      return aria2::shutdown(session->engine, force != 0) == 0
                 ? ARIA2_C_OK
                 : ARIA2_C_ENGINE_ERROR;
    }
    catch (...) {
      session->failed = true;
      return ARIA2_C_ENGINE_ERROR;
    }
  }
  catch (...) {
    return ARIA2_C_ENGINE_ERROR;
  }
}

extern "C" int aria2_c_finalize(aria2_c_session** session, int* result)
{
  if (!session || !result) {
    return ARIA2_C_INVALID_ARGUMENT;
  }
  *result = 0;
  try {
    std::unique_lock<std::mutex> lock(ownerMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
      return ARIA2_C_BUSY;
    }
    auto status = checkOwner(*session);
    if (status != ARIA2_C_OK) {
      return status;
    }
    if (!(*session)->stopped && !(*session)->failed) {
      return ARIA2_C_NOT_STOPPED;
    }
    auto* owner = *session;
    bool failed = owner->failed;
    try {
      *result = aria2::sessionFinal(owner->engine);
    }
    catch (...) {
      failed = true;
    }
    delete owner;
    activeSession = nullptr;
    *session = nullptr;
    try {
      if (aria2::libraryDeinit() != 0) {
        failed = true;
      }
    }
    catch (...) {
      failed = true;
    }
    return failed ? ARIA2_C_ENGINE_ERROR : ARIA2_C_OK;
  }
  catch (...) {
    return ARIA2_C_ENGINE_ERROR;
  }
}
