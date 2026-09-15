/* SPDX-License-Identifier: GPL-2.0-or-later WITH OpenSSL-exception */
#include <aria2/aria2_c.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdatomic.h>
#include <time.h>

static aria2_c_session* owner;
static atomic_int rpc_done;
static unsigned short rpc_port;

static void signal_handler(int value) { (void)value; }

static void* wrong_thread(void* arg)
{
  int running;
  int result;
  aria2_c_session* copy = owner;
  (void)arg;
  assert(aria2_c_run_once(owner, &running) == ARIA2_C_WRONG_THREAD);
  assert(aria2_c_shutdown(owner, 0) == ARIA2_C_WRONG_THREAD);
  assert(aria2_c_finalize(&copy, &result) == ARIA2_C_WRONG_THREAD);
  assert(copy == owner);
  return NULL;
}

static void stop_and_finalize(void)
{
  int running = 1, result = -1, steps = 0;
  assert(aria2_c_finalize(&owner, &result) == ARIA2_C_NOT_STOPPED);
  assert(aria2_c_shutdown(owner, 2) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_shutdown(owner, 0) == ARIA2_C_OK);
  assert(aria2_c_shutdown(owner, 1) == ARIA2_C_OK);
  while (running && steps++ < 100) {
    assert(aria2_c_run_once(owner, &running) == ARIA2_C_OK);
  }
  assert(!running);
  assert(aria2_c_shutdown(owner, 0) == ARIA2_C_OK);
  assert(aria2_c_finalize(&owner, &result) == ARIA2_C_OK);
  assert(!owner);
  assert(result >= 0);
  assert(aria2_c_finalize(&owner, &result) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(NULL, 0, &owner) ==
         ARIA2_C_PROCESS_RESTART_REQUIRED);
  assert(!owner);
}

static int choose_port(void)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {0};
  socklen_t length = sizeof(addr);
  assert(fd >= 0);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  assert(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0);
  assert(getsockname(fd, (struct sockaddr*)&addr, &length) == 0);
  close(fd);
  return ntohs(addr.sin_port);
}

static void rpc_request(int authenticated)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr = {0};
  struct timeval timeout = {5, 0};
  char request[1024], response[8192];
  const char* body =
      authenticated
          ? "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"aria2.getVersion\","
            "\"params\":[\"token:c-api-public-test-fixture-not-a-secret\"]}"
          : "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"aria2.getVersion\","
            "\"params\":[]}";
  size_t used = 0;
  ssize_t amount;
  assert(fd >= 0);
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(rpc_port);
  assert(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0);
  int length = snprintf(
      request, sizeof(request),
      "POST /jsonrpc HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n"
      "Content-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s",
      strlen(body), body);
  assert(length > 0 && (size_t)length < sizeof(request));
  while (used < (size_t)length) {
    amount = send(fd, request + used, (size_t)length - used, 0);
    assert(amount > 0);
    used += (size_t)amount;
  }
  used = 0;
  while ((amount = recv(fd, response + used, sizeof(response) - used - 1, 0)) >
         0) {
    used += (size_t)amount;
    assert(used < sizeof(response) - 1);
  }
  assert(amount == 0);
  response[used] = '\0';
  assert(strstr(response, authenticated ? "\"result\"" : "\"error\""));
  close(fd);
}

static void* rpc_client(void* arg)
{
  (void)arg;
  rpc_request(0);
  rpc_request(1);
  atomic_store(&rpc_done, 1);
  return NULL;
}

int main(int argc, char** argv)
{
  int running = -1;
  pthread_t thread;
  struct sigaction action = {0}, observed = {0};
  aria2_c_session* second = NULL;
  aria2_c_option options[] = {{"enable-dht", "false"},
                              {"enable-dht6", "false"},
                              {"enable-peer-exchange", "false"}};
  aria2_c_option invalid[] = {{"rpc-listen-port", "0"}};
  aria2_c_option missing_secret[] = {{"enable-rpc", "true"}};
  aria2_c_option unknown[] = {{"not-an-aria2-option", "private-fixture"}};
  if (argc > 1 && strcmp(argv[1], "--init-failure") == 0) {
    assert(argc == 3);
    aria2_c_option failure[] = {{"input-file", argv[2]}};
    assert(aria2_c_initialize(failure, 1, &owner) == ARIA2_C_INIT_FAILED);
    assert(!owner);
    assert(aria2_c_initialize(NULL, 0, &owner) ==
           ARIA2_C_PROCESS_RESTART_REQUIRED);
    assert(!owner);
    puts("C ABI: session-init failure cleaned once, terminal restart-required "
         "reject OK");
    return 0;
  }
  assert(aria2_c_abi_version() == 1);
  assert(aria2_c_initialize(NULL, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(!owner);
  assert(aria2_c_initialize(missing_secret, 1, &owner) ==
         ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(unknown, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(invalid, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  aria2_c_option public_rpc[] = {{"rpc-listen-all", "true"}};
  aria2_c_option duplicate[] = {{"dir", "fixture"}, {"dir", "fixture2"}};
  aria2_c_option config[] = {{"conf-path", "private-fixture"}};
  aria2_c_option log[] = {{"log", "private-fixture"}};
  assert(aria2_c_initialize(public_rpc, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(duplicate, 2, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(config, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_initialize(log, 1, &owner) == ARIA2_C_INVALID_ARGUMENT);
  assert(aria2_c_run_once((aria2_c_session*)(uintptr_t)1, &running) ==
         ARIA2_C_INVALID_ARGUMENT);
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  assert(sigaction(SIGINT, &action, NULL) == 0);
  int lifecycle_only = argc > 1 && strcmp(argv[1], "--lifecycle-only") == 0;
  char port[16];
  rpc_port = (unsigned short)choose_port();
  snprintf(port, sizeof(port), "%u", rpc_port);
  aria2_c_option rpc_options[] = {
      {"enable-rpc", "true"},
      {"rpc-listen-port", port},
      {"rpc-secret", "c-api-public-test-fixture-not-a-secret"},
      {"enable-dht", "false"},
      {"enable-dht6", "false"}};
  assert(aria2_c_initialize(lifecycle_only ? options : rpc_options,
                            lifecycle_only ? 3 : 5, &owner) == ARIA2_C_OK);
  assert(aria2_c_initialize(options, 3, &second) == ARIA2_C_BUSY);
  assert(!second);
  assert(sigaction(SIGINT, NULL, &observed) == 0);
  assert(observed.sa_handler == signal_handler);
  assert(pthread_create(&thread, NULL, wrong_thread, NULL) == 0);
  assert(pthread_join(thread, NULL) == 0);
  if (!lifecycle_only) {
    assert(pthread_create(&thread, NULL, rpc_client, NULL) == 0);
    int steps = 0;
    while (!atomic_load(&rpc_done) && steps++ < 10000) {
      assert(aria2_c_run_once(owner, &running) == ARIA2_C_OK);
      assert(running);
      // RUN_ONCE may drain no-wait work without polling; yield to the client.
      const struct timespec yield = {0, 1000000};
      nanosleep(&yield, NULL);
    }
    assert(atomic_load(&rpc_done));
    assert(pthread_join(thread, NULL) == 0);
  }
  stop_and_finalize();
  if (!lifecycle_only) {
    puts("C ABI: validation, single lifecycle, terminal reject, thread/signal "
         "ownership, authenticated loopback RPC OK");
  }
  return 0;
}
