/* SPDX-License-Identifier: GPL-2.0-or-later WITH OpenSSL-exception */
#ifndef ARIA2_C_H
#define ARIA2_C_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ABI v1. All strings are borrowed NUL-terminated bytes, copied during init.
 * No string or allocator-owned value is returned. See README.ohos for the
 * single-lifecycle/single-thread contract and limits. */
typedef struct aria2_c_session aria2_c_session;
typedef struct aria2_c_option {
  const char* name;
  const char* value;
} aria2_c_option;

enum aria2_c_status {
  ARIA2_C_OK = 0,
  ARIA2_C_INVALID_ARGUMENT = -1,
  ARIA2_C_BUSY = -2,
  ARIA2_C_WRONG_THREAD = -3,
  ARIA2_C_INIT_FAILED = -4,
  ARIA2_C_ENGINE_ERROR = -5,
  ARIA2_C_NOT_STOPPED = -6,
  ARIA2_C_NO_MEMORY = -7,
  ARIA2_C_PROCESS_RESTART_REQUIRED = -8
};

uint32_t aria2_c_abi_version(void);
/* Initializes the process-global library and one keep-running session.
 * A second instance, or mixing this facade with the C++ API/CLI in the same
 * process, is forbidden. Only one successful lifecycle is allowed per process;
 * after finalize or failure after global init, restart in a new process.
 * Prevalidation errors do not consume the lifecycle. out_session is cleared
 * on failure. */
int aria2_c_initialize(const aria2_c_option* options, size_t option_count,
                       aria2_c_session** out_session);
/* One upstream RUN_ONCE invocation, including its internal no-wait draining.
 * The idle poll can wait up to ~1 second; this is NOT a hard time deadline.
 * out_running is 1 while alive and 0 after engine/session save completion. */
int aria2_c_run_once(aria2_c_session* session, int* out_running);
/* Same owner thread only. Continue run_once until out_running==0.
 * force is exactly 0 or 1; graceful shutdown may be escalated to force. */
int aria2_c_shutdown(aria2_c_session* session, int force);
/* Only after stopped (or ENGINE_ERROR). Releases session and global library;
 * clears *session. out_engine_result is the upstream aria2 exit/result code. */
int aria2_c_finalize(aria2_c_session** session, int* out_engine_result);

#ifdef __cplusplus
}
#endif
#endif
