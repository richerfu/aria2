# OHOS shared aria2 implementation evidence

Date: 2026-09-15. Status: **implemented; owner checks complete; independent
acceptance and G0-B remain pending**.

The user's subsequent direct-output requirement supersedes the build1 symlink
layout below. Current ordinary-file build2 delivery and verification are recorded
in the build2 section; build1 is retained only as historical evidence, not the
current package or its source identity.

The later **build3 provenance-only repair** below is now the current freeze.
Build1/build2 stages, source snapshots, source lists and packages are preserved
byte-for-byte as historical evidence. Their engine/CLI bytes are unchanged;
their metadata/source/package hashes are not substituted for build3.

Assigned implementation owner: GPT-5.6 Sol `xhigh`, existing `plan_acceptance`
worker reassigned from its earlier planning-review task. This worker authored
this change and cannot independently accept it. No new GPT-6 Astra planning
approval or fresh Sol acceptance is claimed. Hotrix ADR 0001 records the
upstream once-per-process contract and the remaining planning handoff.

## Scope and source identity

- Worktree: `/Users/ranger/Desktop/project/southorange/aria2-hotrix-shared`.
- Branch: `hotrix/ohos-shared-library`.
- Base HEAD: `3f723c1f9a53448f3809fa4f378221c9fef11dc0`, aria2 1.37.0.
- Changes are **uncommitted**. The base commit alone is not the source identity
  of the new artifacts; the authoritative build-source snapshot and file hashes
  include the new, untracked C API/header/test/scripts.
- Historical build1 source-file inventory: 1,407 files.
- SHA-256 of `evidence/source-files.sha256`:
  `a76c920a49915c343503ea07f66cac87a611f9c85c612ef3d270487b1b924fa2`.
- This post-build report is intentionally excluded from that source list/hash
  and snapshot to avoid self-referential reporting. The manifest records this
  exclusion. The source status/diff represent the snapshot time before this
  report was created. The later build2 packaging-only correction has its own
  newly frozen source identity in the build2 section.

The original `/Users/ranger/Desktop/project/southorange/aria2` was inspected
again after packaging: branch `ohos`, same base HEAD above, clean status. It was
not edited, reset, committed, merged or pushed. The OpenSSL repository was
read-only and remains clean. No device engine installation or Hotrix product
implementation occurred.

Changed paths relative to the base:

```text
.gitignore
.dockerignore                         (new)
Dockerfile.ohos
Makefile.am
README.ohos
configure.ac
ohos-config
ohos-release
ohos/generic-config
ohos/zlib-config
ohos/artifact-manifest                (new)
ohos/artifact-layout-test             (new; build2)
ohos/build-deps                       (new)
ohos/dependencies.sha256              (new)
ohos/host-c-api-test                  (new)
src/Makefile.am
src/Platform.cc
src/aria2api.cc
src/aria2capi.cc                      (new)
src/includes/Makefile.am
src/includes/aria2/aria2_c.h          (new)
test/aria2_c_lifecycle.c              (new)
docs/ohos-shared-library-implementation.md (new; excluded evidence report)
```

`src/prefs.cc` and `src/prefs.h` have **no diff**. A speculative four-line
preference-table lifetime redesign was withdrawn; upstream globals were not
made reentrant. `src/Platform.cc` only adds the matching `ENABLE_SSL` guard
around teardown calls whose declarations are already guarded in SocketCore.
`src/aria2api.cc` clears its platform pointer after deletion for safe failed-init
cleanup; it does not authorize repeated global initialization.

## Current build3 — ARIA2-PROV-01 provenance repair, frozen

Repair owner: actual GPT-5.6 Sol `xhigh`, fresh session
`/root/aria2_provenance_repair`; this author cannot accept the repair.
Independent acceptance of this new freeze remains pending.

The pinned OpenSSL prebuilt header actually reports
`OpenSSL 1.1.1x-dev  xx XXX xxxx`, with `OPENSSL_VERSION_NUMBER=0x10101180L`.
The earlier report's 1.1.1w label and release-named upstream notice were wrong.
This repair changes provenance labels only: no OpenSSL source/binary upgrade,
downgrade, protocol/security behavior change or engine/C ABI source change.
The port revision remains `0f7849a999d3c69f8d2ed23bf5f8d680a2aac6be`; its
upstream gitlink and license URL remain pinned at
`b372b1f76450acdfed1e2301a39810146e28b02c`.

Both native and Docker dependency metadata derive `version_header` from the
actual header. Manifest generation validates the pinned port/upstream revisions,
exact header bytes and metadata equality, rejects any assigned release-version
field, and validates the exact unchanged upstream-license bytes. New stages
carry ordinary `notices/OpenSSL-LICENSE.txt` and `notices/OpenSSL-opensslv.h`,
with their paths/hashes and the derived number header in dependencies/manifest.
The old PIC prefix's historical notice filename is accepted only as an input
compatibility case: only the new staged copy is normalized. The staged source-pin
notice is refreshed with the neutral filename; no old prefix file is touched.

Output: `ohos-out/arm64-v8a-delivery-build3`.
Package stem in `ohos-packages/`:
`aria2-1.37.0-aarch64-linux-ohos-shared-build3`.

| Current artifact | SHA-256 |
|---|---|
| `lib/libaria2.so` (ordinary compiler/staged file) | `10e2d3cd0c39495354d023bcd0443698e096f0744fe8aa6176c547199b908998` |
| `aria2c` | `d22558644432fc3c6fc3b9758fd647ac638b3abc8a59ef952a4d3e55f81d85c8` |
| `lib/libc++_shared.so` | `f69947af100b652e349ae51e4a4888bcc7a327db9085272d318176ef1782bf0f` |
| `notices/OpenSSL-LICENSE.txt` (unchanged bytes) | `c32913b33252e71190af2066f08115c69bc9fddadf3bf29296e20c835389841c` |
| `notices/OpenSSL-opensslv.h` | `0035c267e11ba05295dcf7af3044a18b16665d5f0489e298263584b6952042c1` |
| `dependencies.json` | `3410741ce3dae44f2b185959fc2c7a6215e28afa52c82eb1cb81e249b3edefc5` |
| `evidence/source-files.sha256` (1,409 files) | `51a3e7314ba7a33774271000bfcf24f6bd8b456a34b7b4c24148e5033fe1e9d2` |
| `source-snapshot.tar.gz` | `bc199f0caa6bde07ffefda596ac38acc2ed309ce5813d5e42ef99ee7561d3f52` |
| `evidence/source-diff.patch` | `0f0b4e41dd4ddafd6381957bc230c35d33220ec0b8be6ad1060297f9b5003304` |
| `artifact-manifest.json` | `f61718f2a4992ac651c517d09420b6ac51f3de1f60886c7c912eb4efaf25800b` |
| `SHA256SUMS` (58 entries) | `03afceac3df3bcf844cfad5a5a92680335294e7d63bd3cdb46832a4bf497782b` |
| build3 ZIP | `6644ff7c1d576d0a27d1267e548c9085e5a94b17bea8da25139373fee21c531b` |
| build3 tar.gz | `146a7bd0cf141ce3ff0a954bea83bd7c143aa520f52d2b89ed0dcc3a053ea536` |
| build3 `packages.sha256` | `3b48daaad41f4a3bfe56ef01ecd6b0b529c44713c9fe860ac233149eed8d2b1c` |

The exact build2-to-build3 source inventory delta is only five modified producer
files plus the new regression script below. This excluded owner report is the
seventh repair path and is updated after the new snapshot to avoid self-reference.

| Repair source path | SHA-256 |
|---|---|
| `Dockerfile.ohos` | `b4284782bdf9557c1ee3b3c780ea27df4941b5f0e2ce4f8271b3a8c886a8c5dc` |
| `ohos/build-deps` | `661e967322b355e13a2ee3ec73c10239c4c1a042a25e6eb94bc0a3f45f1e9507` |
| `ohos/dependencies.sha256` | `67b9ba88cd803fde144fc23835a7ba88024d73b57ca3027d8ea5a090ca562fc2` |
| `ohos/artifact-manifest` | `926d68d91438ed0f9ea38ff2de690487c23a3ee204991d666f7fe9f85cf707b2` |
| `README.ohos` | `850e99f22db539b30c7b4927f105d03cc2e516c9bc13aa40523215981b7901d1` |
| `ohos/openssl-provenance-test` (new) | `e3b157d3d9045ce15f698dd7ad400f324ff84a8b322402e0406f940aab94f139` |

The original 436-line report was copied before any report edit to
`ohos-out/aria2-provenance3-inputs/evidence/owner-report-build2-original.md`;
its verified original SHA-256 remains
`4abaca17e212528bb0184b242ae78b0e99b01a1373572e956446b5caa8223d37`.
A matching backup is included as build3 `evidence/owner-report-build2-original.md`.
Before/after verification preserves all 196 regular files across both historical
stages/package directories/archives/hash lists, all 1,479 regular PIC-prefix
files, all 1,278 engine/dependency/test source files and retained symlink targets.
The complete source inventory delta independently shows no engine/C ABI/test
consumer/host-script/build-rule change. Preservation lists/logs and the truthful
retained-build declaration are packaged under `evidence/`; source-delta and
post-package evidence remain outside the immutable freeze in the new input
evidence directory.

Actual build3 execution used the same concrete root/native SDK/PIC prefix and
retained `ohos-build/arm64-v8a/run.G81fwy` recorded below, with new
`OHOS_OUT_DIR="$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3"`, then:

```sh
OHOS_NDK_HOME="$ARIA2_SDK" OHOS_DEPS_PREFIX="$ARIA2_ROOT/ohos-deps/arm64-pic" \
OHOS_BUILD_DIR="$ARIA2_ROOT/ohos-build/arm64-v8a/run.G81fwy" \
OHOS_OUT_DIR="$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3" \
JOBS=8 ./ohos-config aarch64
./ohos/openssl-provenance-test "$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3" \
  "$ARIA2_SDK" aarch64-unknown-linux-ohos arm64-v8a
./ohos/artifact-layout-test "$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3" \
  "$ARIA2_SDK" aarch64-unknown-linux-ohos arm64-v8a
# Fresh OHOS C11 consumer compile/link, ELF reports and repair logs were added.
OHOS_DEPS_PREFIX="$ARIA2_ROOT/ohos-deps/arm64-pic" ./ohos/artifact-manifest \
  "$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3" "$ARIA2_SDK" \
  aarch64-unknown-linux-ohos arm64-v8a
OHOS_OUT_DIR="$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build3" \
  BUILD_VER=3 ./ohos-release aarch64
```

These commands completed exit 0. Reconfiguration reran four wslay dependency
objects, four WebSocket objects and final library/CLI links; unchanged objects
were reused. This is not a clean full engine rebuild, host rebuild, container
build or device run. The actual ordinary library/CLI were restaged and hashed;
equal bytes are measured, not assumed from old artifact identities. SDK26 ELF
checks retain zero library entry, `SONAME=libaria2.so`, no TEXTREL, five C exports
and only libc++/libc runtime dependencies. The fresh C11 consumer compiles and
links with `-Wall -Wextra -Werror -Wl,-z,defs`; it is not target-executed.

The new regression executes real manifest generation on a retained scratch
legacy-notice copy, validates derived header/number and exact Docker jq metadata,
and rejects wrong version-header, false release-version field, changed license,
wrong upstream revision and changed header. Its source-stage hashes are unchanged.
The existing four invalid ordinary-layout cases also remain rejected by both
manifest and package entry points. All 1,409 current source hashes were verified;
archive/source paths were checked before fresh scratch extraction. Stage and both
archives pass all 58 checksums and agree byte-for-byte, with no symlink, versioned
library or top-level alias. The exact license bytes/header metadata and missing
API22/G0/security gates agree in each tree.

First-download checksum trust, incomplete upstream source/patch offer,
dependency security/license review, API22 `validated_min_api=null`, device/HAP,
native authenticated RPC/storage/smoke/soak, G0-A/B/C, B3 and release obligations
remain pending. K0 remains fully paused. No Hotrix integration contract, product,
probe, Grok/UI, K0, bindings, OpenSSL dependency checkout, original `ohos` branch,
installed engine or signing material is edited. No commit/merge/push occurs.

## Historical build2 — direct ordinary-file output, frozen

Output: `/Users/ranger/Desktop/project/southorange/aria2-hotrix-shared/ohos-out/arm64-v8a-delivery-build2`.
Packages: `/Users/ranger/Desktop/project/southorange/aria2-hotrix-shared/ohos-packages/aria2-1.37.0-aarch64-linux-ohos-shared-build2.{zip,tar.gz}`.

| Historical build2 artifact | SHA-256 |
|---|---|
| `lib/libaria2.so` | `10e2d3cd0c39495354d023bcd0443698e096f0744fe8aa6176c547199b908998` |
| `aria2c` | `d22558644432fc3c6fc3b9758fd647ac638b3abc8a59ef952a4d3e55f81d85c8` |
| `lib/libc++_shared.so` | `f69947af100b652e349ae51e4a4888bcc7a327db9085272d318176ef1782bf0f` |
| `evidence/source-files.sha256` (1,408 files) | `1e7fdb55eb31fe708dfff77faa2682b1638ee4c6dae71e63efeb71096ddb71e9` |
| `source-snapshot.tar.gz` | `3fa77fb508b20a272fe574382fd3c8eca7cbd15e759fa159a83d2bb44c637655` |
| `artifact-manifest.json` | `9c0b6e75d62919044d690ddf5ca779863b2210183f4cf9066369319c698ba241` |
| `SHA256SUMS` (47 regular-file entries) | `93e7a70aedffc3312f670b52312a49616494bd42bb35e9069a30c5da8f68b6d4` |
| build2 ZIP | `8ccfefc118fa4c9b8e8557c8687ff6a456fa24435c2632420805d382ef987415` |
| build2 tar.gz | `a69ef891844aef84431d0d31978733ebf0ff38a9e9b2102d6c0112d5bc52ac7c` |

The actual libtool compile output
`ohos-build/arm64-v8a/run.G81fwy/src/.libs/libaria2.so` is already an ordinary
unversioned file. The existing OHOS `-avoid-version -Wl,-soname,libaria2.so`
target needs no change; non-OHOS `-version-info` behavior is untouched. The new
staging logic copies that output directly to `lib/libaria2.so` and strips only
debug information. It no longer renames the library to a versioned filename or
uses `ln -s`. There is no top-level convenience library alias. The library/CLI
ELF hashes happen to equal build1 because engine/CLI code did not change; they
were computed again from build2, not substituted from build1. New source,
manifest, snapshot and package hashes above supersede their historical hashes.

Build2 delivery and both extracted packages contain no symlinks and no
`libaria2.so.*` file. SONAME remains `libaria2.so`, so HAP packaging copies the
ordinary file directly; no unpack-time dereferencing is required. Actual HAP
loading is still unperformed. Manifest and release generation enforce this
layout, and `ohos/artifact-layout-test` verifies rejection of four deliberately
invalid retained fixture copies: a library symlink, an unrelated symlink, a
versioned library file and a top-level ordinary convenience alias. Both entry
points reject each before distribution output is created.

Changes specific to the build2 correction are `ohos-config`, `ohos-release`,
`ohos/artifact-manifest`, new `ohos/artifact-layout-test`, `README.ohos` and this
excluded report. No C ABI/engine/global/process API redesign occurred. The
retained build was reconfigured and its four WebSocket objects/dependency target
and final links reran; objects with unchanged inputs were reused. Build1 outputs
and packages remain intact. At that freeze, no producer source changed after the build2 source
snapshot; this report alone was updated afterward and is explicitly excluded.

Executed build2 commands used these task-specific paths:

```sh
ARIA2_ROOT=/Users/ranger/Desktop/project/southorange/aria2-hotrix-shared
ARIA2_SDK=/Users/ranger/.meat/ide/DevEco-Studio-26.0.0.821.app/Contents/sdk/default/openharmony/native
ARIA2_DELIVERY="$ARIA2_ROOT/ohos-out/arm64-v8a-delivery-build2"
cd "$ARIA2_ROOT"
OHOS_NDK_HOME="$ARIA2_SDK" \
OHOS_DEPS_PREFIX="$ARIA2_ROOT/ohos-deps/arm64-pic" \
OHOS_BUILD_DIR="$ARIA2_ROOT/ohos-build/arm64-v8a/run.G81fwy" \
OHOS_OUT_DIR="$ARIA2_DELIVERY" JOBS=8 ./ohos-config aarch64
./ohos/artifact-layout-test "$ARIA2_DELIVERY" "$ARIA2_SDK" \
  aarch64-unknown-linux-ohos arm64-v8a
"$ARIA2_SDK/llvm/bin/aarch64-unknown-linux-ohos-clang" \
  -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror \
  -I src/includes -c test/aria2_c_lifecycle.c \
  -o "$ARIA2_DELIVERY/evidence/c-consumer-ohos.o"
"$ARIA2_SDK/llvm/bin/aarch64-unknown-linux-ohos-clang" \
  "$ARIA2_DELIVERY/evidence/c-consumer-ohos.o" \
  -L "$ARIA2_DELIVERY/lib" -laria2 \
  "-Wl,-rpath-link,$ARIA2_DELIVERY/lib" -Wl,-z,defs \
  -o "$ARIA2_DELIVERY/evidence/c-consumer-ohos"
# Target ELF reports and owner build/test logs were added before this final freeze.
./ohos/artifact-manifest "$ARIA2_DELIVERY" "$ARIA2_SDK" \
  aarch64-unknown-linux-ohos arm64-v8a
OHOS_OUT_DIR="$ARIA2_DELIVERY" BUILD_VER=2 ./ohos-release aarch64
```

All steps completed exit 0. Replaying into this populated output/package is
deliberately rejected; use fresh paths for another build. The target plain-C
consumer was compiled and linked again against the current ordinary library;
it was not device-run. The unchanged host consumer was rerun for validation,
single-owner/thread/signal/authenticated RPC, consumed-init failure/terminal
restart-required and 100 independent processes, all exit 0. Its existing host
build is still `host-build/c-api.cE6N5q`; no host rebuild is claimed for this
packaging-only correction.

Final owner checks also completed exit 0:

- shell `sh -n` and `git diff --check`;
- SDK LLVM readelf/nm on the real build2 and extracted library: AArch64 DYN,
  unversioned SONAME, no TEXTREL, five unmangled C ABI exports;
- CLI retains only libc++/libc dynamic dependencies and no libaria2 dependency;
  target consumer links to `libaria2.so` without absolute DT_NEEDED names;
- both ZIP/tar package hashes and each extracted package's 47 SHA256SUMS entries;
- no symlink, versioned library file or top-level alias in stage/ZIP/tar;
- source verification for all 1,408 listed files, including the new test script;
- build/package overwrite attempts each return expected exit 1 and leave the
  before/after library/ZIP hashes unchanged;
- original `ohos` worktree remains clean and `src/prefs.cc/h` remain unchanged.

Logs copied into delivery before freeze: `evidence/aria2-ohos-regular-build2.log`,
`evidence/aria2-regular-layout-build2.log`,
`evidence/aria2-host-c-api-build2-tests.log`; target C object/binary/readelf and
compiled-library ELF/hash evidence are beside them. Post-package checks are
retained outside the immutable candidate:
`/tmp/aria2-regular-build2-extraction.log`,
`/tmp/aria2-regular-build2-no-overwrite-build.log`,
`/tmp/aria2-regular-build2-no-overwrite-package.log`,
`/tmp/aria2-regular-build2-source-verification.log`.
Both extracted verification trees and their readelf/export reports are under
`/tmp/aria2-regular-build2-extract.NiBT9R/{zip,tar}`.

This is the author's bounded ordinary-file-layout verification, not independent
acceptance, G0-B, API22, HAP/device or security/release PASS. Fresh review of this
latest layout slice is requested separately. All remaining integration gates in
the final section below still apply.

## Historical build1 artifacts — retained, superseded

Output directory: `ohos-out/arm64-v8a-delivery/`.

| Artifact | SHA-256 |
|---|---|
| `lib/libaria2.so.1.37.0` | `10e2d3cd0c39495354d023bcd0443698e096f0744fe8aa6176c547199b908998` |
| `aria2c` | `d22558644432fc3c6fc3b9758fd647ac638b3abc8a59ef952a4d3e55f81d85c8` |
| `lib/libc++_shared.so` | `f69947af100b652e349ae51e4a4888bcc7a327db9085272d318176ef1782bf0f` |
| `source-snapshot.tar.gz` | `5bdfd0a5b3485f8922c3129ba103073786fed58b3fcd1a6c87817be2165bbad3` |
| `artifact-manifest.json` | `d303a392be22914512d84b9ad6529500d6832750c6a09941eb2bc9941321f94b` |
| `SHA256SUMS` | `96d49a8ac134511fe26b899e66e8245bd0ae5a5ce9e1ab784c40f3060d25fcd2` |
| `ohos-packages/aria2-1.37.0-aarch64-linux-ohos-shared-build1.zip` | `53f23ee36d5ec367051682731978b75cdc9a63919cf4f9d566d3160071af7b79` |
| `ohos-packages/aria2-1.37.0-aarch64-linux-ohos-shared-build1.tar.gz` | `a1402db9394612ae7954d55dc223a04ed0b6b058656b5b79248b63368f33ec4d` |

The old build1 package retains `lib/libaria2.so -> libaria2.so.1.37.0` and a top-level
`libaria2.so -> lib/libaria2.so`. The actual ELF SONAME is **libaria2.so**, not
the versioned filename. HAP packaging must dereference the symlink to its
unversioned ABI library path; that old requirement is removed by build2's direct
ordinary-file output. HAP loading has not been tested here.

The shared library is ELF64 little-endian AArch64 DYN, has no TEXTREL, and exports
five unmangled C symbols:

```text
aria2_c_abi_version
aria2_c_initialize
aria2_c_run_once
aria2_c_shutdown
aria2_c_finalize
```

Both engine library and CLI need only `libc++_shared.so` and system `libc.so`.
The CLI statically embeds the engine, remains PIE, and does **not** acquire a
libaria2 dynamic dependency. The OHOS plain-C consumer instead needs the correct
SONAME `libaria2.so`. No absolute local/build path appears in these DT_NEEDED
names. No `OH_Ability_*` import or child-process API/shim was added.

The output includes `artifact-manifest.json`, `SHA256SUMS`, headers, complete
aria2/SDK/expat/zlib/c-ares/libssh2/OpenSSL license notices, ELF/exports/undefined
symbol reports, copied owner-test/build logs, base/diff/status, per-source-file
hashes and `source-snapshot.tar.gz`. The tracked Git delta does not contain
untracked additions by itself; the full snapshot and per-file list do.

## C lifecycle contract

`src/includes/aria2/aria2_c.h` is the stable ABI v1 boundary: only integers,
size_t, borrowed string pointers, an option-array C record and a local opaque
owner pointer. No std:: type, allocator-owned returned string, Rust type or
cross-process pointer is exposed. All strings are copied during initialize;
caller memory can be released after it returns. All fallible exported functions
contain C++ exceptions and return numeric typed status; they do not return
private diagnostic text. Caller pointers must be valid memory.

There is one global lifecycle/session per process. `keepRunning=true` keeps an
empty RPC engine alive; `useSignalHandler=false` preserves host signal handlers.
All lifecycle calls use the creating thread; concurrent use returns BUSY and
other threads return WRONG_THREAD. This is not upstream Session thread safety.

Pure option prevalidation occurs before libraryInit and permits corrected input.
Once global initialization is attempted, an initialization failure consumes the
process lifecycle. Successful finalize also consumes it. Later initialize
returns `ARIA2_C_PROCESS_RESTART_REQUIRED` **before accessing torn-down globals**.
Restart means a new process, not a new Session in the same failed/finalized
process. Session/global cleanup is attempted at most once.

RUN_ONCE is upstream iteration with idle polling up to approximately one second
and internal no-wait draining; it is not a hard deadline. Shutdown is explicit,
same-owner-thread, then iteration until running=0, then finalize. The caller may
escalate to force and its process supervisor may terminate a stuck child.

RPC options are supplied in memory. The facade forces no-conf/quiet/no file
logger/loopback-only and rejects config/log/public-listener overrides, duplicates,
unknown/invalid options and unauthenticated RPC initialization. The caller supplies
32..1024 printable token bytes and owns entropy. Port 0 is unsupported; the caller
chooses 1024..65535 and authenticated-probes before Ready. A consumed-init bind
failure requires a fresh process with a bounded retry. Secrets are not put in
argv/config/log by this facade. Upstream heap-string zeroization is not guaranteed;
credential-bearing download URL/session persistence needs a separate accepted
encrypted-source integration contract.

## Build inputs and historical build1 commands

Actual cross-build used the **macOS** DevEco Studio 26.0.0.821 native SDK:
`/Users/ranger/.meat/ide/DevEco-Studio-26.0.0.821.app/Contents/sdk/default/openharmony/native`.
Its metadata reports API/platform 26. The Docker path needs a Linux SDK and was
not run with this Mach-O LLVM. Docker now shares PIC policy, pinned dependency
hashes, notices and shared packaging. A non-Git Docker context declares its base
separately and hashes its full context; it is not claimed to be a clean Git
revision. That container path remains unexecuted.

Target dependency versions are the supplied port baseline, not upgrades:
OpenSSL prebuilt producer revision (historical report label `1.1.1w` corrected
by ARIA2-PROV-01 to the actual `1.1.1x-dev` header; see build3 above)
`0f7849a999d3c69f8d2ed23bf5f8d680a2aac6be`, upstream gitlink
`b372b1f76450acdfed1e2301a39810146e28b02c`; expat 2.5.0; zlib 1.3.1;
c-ares 1.21.0; libssh2 1.11.0. Source/license SHA-256 pins are in
`ohos/dependencies.sha256` and explicitly describe first-download trust, not
authenticated vendor hashes. All new dependency builds use PIC. Final shared
link with `-z defs` and absence of TEXTREL validate compatibility of the supplied
OpenSSL static objects with this shared build.

The commands below show the actual retained inputs and successful build steps
after the corrections listed next; the task-specific variable definitions spell
out the absolute paths rather than placeholders. They are historical build
evidence, not instructions to overwrite the existing delivery: rerunning the
dependency/build/package scripts against these populated outputs deliberately
rejects them. Use fresh output/prefix paths for a new build. Build/log directories
were retained; scripts do not recursively delete them.

```sh
ARIA2_ROOT=/Users/ranger/Desktop/project/southorange/aria2-hotrix-shared
ARIA2_SDK=/Users/ranger/.meat/ide/DevEco-Studio-26.0.0.821.app/Contents/sdk/default/openharmony/native
ARIA2_DEPS="$ARIA2_ROOT/ohos-deps/arm64-pic"
ARIA2_BUILD="$ARIA2_ROOT/ohos-build/arm64-v8a/run.G81fwy"
ARIA2_DELIVERY="$ARIA2_ROOT/ohos-out/arm64-v8a-delivery"
cd "$ARIA2_ROOT"
LIBTOOLIZE=glibtoolize autoreconf -fi
OHOS_NDK_HOME="$ARIA2_SDK" \
OHOS_OPENSSL_REPO=/Users/ranger/Desktop/project/southorange/dioxus_in_harmony/ohos-openssl \
OHOS_DEPS_PREFIX="$ARIA2_DEPS" \
JOBS=8 ./ohos/build-deps

OHOS_NDK_HOME="$ARIA2_SDK" \
OHOS_DEPS_PREFIX="$ARIA2_DEPS" \
OHOS_BUILD_DIR="$ARIA2_BUILD" \
OHOS_OUT_DIR="$ARIA2_DELIVERY" \
JOBS=8 ./ohos-config aarch64

"$ARIA2_SDK/llvm/bin/clang-format" -i src/aria2capi.cc src/includes/aria2/aria2_c.h test/aria2_c_lifecycle.c
make -C "$ARIA2_BUILD/src" -j8
# Final rebuilt shared library/CLI were staged and stripped; headers refreshed.
./ohos/artifact-manifest "$ARIA2_DELIVERY" "$ARIA2_SDK" aarch64-unknown-linux-ohos arm64-v8a
OHOS_OUT_DIR="$ARIA2_DELIVERY" ./ohos-release aarch64
```

The final target reused a retained, isolated build for narrow fixes after an
initial fresh configure/compile. No stale object from another worktree or the
original ohos branch was imported. Fresh prefixes/output/build directories are
supported by the documented scripts. A second clean-checkout bit-for-bit build
comparison was not performed; artifact hashes identify this actual build.

## Owner verification and corrected failures

| Check | Actual result |
|---|---|
| `git diff --check`, shell `sh -n` | exit 0 |
| plain `cc -std=c11 -Wall -Wextra -Werror -pthread` consumer compilation | exit 0; not C++ compilation |
| libtool link using explicit `--tag=CC` to real host shared library | exit 0 |
| host validation, active-instance/owner-thread/signal checks, unauthenticated/authenticated loopback RPC | exit 0 |
| session-init failure, cleanup once, terminal restart-required reject | exit 0 in its own consumer process |
| 100 independent consumer processes, each single lifecycle plus terminal reject | exit 0; not 100 reinitializations in one process |
| host CLI `--version` | exit 0 |
| OHOS C11 consumer compile/link against actual libaria2.so | exit 0; executable not device-run |
| OHOS LLVM readelf/nm, SONAME, DT_NEEDED, no TEXTREL, five C exports | exit 0 |
| ZIP/tar SHA verification; extracted ZIP SHA256SUMS | exit 0 |
| extracted ELF SONAME/no TEXTREL/C exports | exit 0 |
| rerun build/package into existing final outputs | expected exit 1; explicit rejection and before/after library/ZIP hashes unchanged |
| final source-file SHA verification including untracked producer files | exit 0 |

Host evidence directory is `host-build/c-api.cE6N5q`. Its last test commands:

```sh
cd /Users/ranger/Desktop/project/southorange/aria2-hotrix-shared/host-build/c-api.cE6N5q
make -C src -j8
cc -std=c11 -Wall -Wextra -Werror -pthread -I../../src/includes \
  -c ../../test/aria2_c_lifecycle.c -o c-consumer.o
./libtool --tag=CC --mode=link cc -pthread c-consumer.o src/libaria2.la -o c-consumer
./c-consumer
./c-consumer --init-failure "$PWD/missing-uri-list.fixture"
for cycle in $(seq 1 100); do ./c-consumer --lifecycle-only || exit 1; done
src/aria2c --version
```

The target consumer compile/link used these concrete arguments (with the
task-specific path variables defined above):

```sh
cd "$ARIA2_ROOT"
"$ARIA2_SDK/llvm/bin/aarch64-unknown-linux-ohos-clang" \
  -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror \
  -I src/includes -c test/aria2_c_lifecycle.c \
  -o "$ARIA2_DELIVERY/evidence/c-consumer-ohos.o"
"$ARIA2_SDK/llvm/bin/aarch64-unknown-linux-ohos-clang" \
  "$ARIA2_DELIVERY/evidence/c-consumer-ohos.o" \
  -L "$ARIA2_DELIVERY/lib" -laria2 \
  "-Wl,-rpath-link,$ARIA2_DELIVERY/lib" -Wl,-z,defs \
  -o "$ARIA2_DELIVERY/evidence/c-consumer-ohos"
```

Its object, executable and readelf output are retained under delivery `evidence/`;
the executable was not run on a target device.

Earlier failures are not counted as passes:

- Host no-TLS build exposed unguarded TLS teardown calls; fixed by matching the
  existing declaration guard. `/tmp/aria2-host-c-api-build.log` retains failure.
- Plain-C libtool link initially lacked a tag; explicit CC tag fixed it.
  `/tmp/aria2-host-c-api-build-fixed.log` retains failure.
- Repeated same-process global lifecycle caused a real use-after-free/double
  delete. LLDB stopped in `PrefFactory::~PrefFactory -> deletePrefResource ->
  Platform::tearDown -> libraryDeinit`. The contract now follows upstream once
  per process; preference redesign was withdrawn and terminal tests pass.
- RPC harness initially exhausted 30 no-wait iterations before its client
  thread could run. A bounded client yield/longer iteration limit fixed the
  test harness without changing DownloadEngine polling semantics.
- OHOS libtool initially omitted SONAME; explicit `-Wl,-soname,libaria2.so`
  fixed it; the owner rechecked the extracted package's actual ELF afterward.
- CLI static linking lost private `.la` archive dependencies; its OHOS target
  now explicitly appends EXTLDADD. It links and retains no libaria2 dependency.
- Static CLI is a real `src/aria2c`, not a `.libs` wrapper; staging handles both.
- OHOS C consumer initially needed `<sys/time.h>` for timeval; header corrected,
  then plain-C target compile/link reran successfully.
- Piping the large nm output to grep-q produced harmless broken-pipe diagnostics
  in an initial extraction check; it was rechecked via a saved exports file,
  with no pipe errors and exit 0.

Final logs copied into delivery evidence include owner host build/tests,
target delivery build and final-format rebuild. Extraction and rejection logs:
`/tmp/aria2-shared-package-extraction.log`, `/tmp/aria2-shared-extracted-elf.log`,
`/tmp/aria2-shared-extracted-c-symbols.txt`,
`/tmp/aria2-shared-no-overwrite-build.log`,
`/tmp/aria2-shared-no-overwrite-package.log`,
`/tmp/aria2-shared-final-source-verification.log`.
Extracted verification directory:
`/tmp/aria2-shared-extract.foKWcQ/aria2-1.37.0-aarch64-linux-ohos-shared-build1`.

## Remaining gates and rollback

The manifest requests minimum API22 but records `validated_min_api=null` and
API22 **MISSING**. The available SDK26 sysroot has no independently verified
API22 symbol baseline. SDK26 link success is not API22 proof. Phone API26 smoke
would not change this conclusion. API22 undefined-symbol/runtime checks and
actual HAP child-load/lifecycle/RPC remain required.

No target device, storage/download/recovery/soak, security-release, other ABI or
independent fresh-review PASS is claimed. Existing upstream CppUnit tests were
not run because CppUnit was unavailable; the independent plain-C tests and
actual target artifact checks are the bounded owner evidence here.
OpenSSL's exact license is included, but producer upstream source/patch
archive/source-offer responsibility remains incomplete before redistribution.
The older supplied dependency baseline still requires security/license release
review. Secret heap erasure and encrypted credential-bearing URL persistence
remain explicit integration gaps, not silently solved by this adapter.

Rollback boundary is this isolated uncommitted producer branch/worktree and its
retained candidate outputs. No original branch, dependency branch, user data,
installed app, signing material or RPC secret was changed. Hand these exact
source/artifact/package hashes to a fresh independent Sol xhigh reviewer; do
not mark G0-B or product readiness PASS from the author's owner checks.
