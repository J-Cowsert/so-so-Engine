# Development validation

Run commands from the repository root. `CMakeLists.txt`, `cmake/Engine.cmake`, and
the current workflows are authoritative when build instructions disagree. Use
separate build directories for different configurations/toolchains; do not destroy
another developer's build tree to obtain a clean result.

## Current baseline

The project defines C++23 static libraries `soso-core` and `so-so`, and a client
executable, `Sandbox`. CMake 3.25 or newer is required. Linux and Windows have Debug
and Release CI jobs that build and run core tests before uploading artifacts.
A separate Linux Clang job runs headless core tests under ASan and UBSan. Dist is
also declared by CMake. Graphics acceptance and performance checks remain manual.

The initial core test suite covers UID value/hash behavior, owned layer names,
and resource identity comparisons. It has six doctest cases exposed as one CTest
entry, `soso.core`. Tests are enabled by default with `BUILD_TESTING=ON`; test
dependencies are vendored and never downloaded during configuration/build. The
missing UID include, dangling layer-name reference, and recursive resource
inequality have regression coverage. This is initial coverage, not verification of
the entire engine, rendering, lifecycle, or concurrency.

- Tracy is linked, but `SS_ENABLE_PROFILING` in `Core/Profiler.h` is `0`.
  Verify instrumentation is active before interpreting a profiling capture.
- `TestLayer` is an interactive demo, not an automated graphics test suite.

Recheck these observations before relying on them and update this section as the
gaps are repaired. A zero-test run does not validate behavior. Distinguish observed
results from configured CI checks; a workflow definition does not establish that a
particular commit passed remote CI.

## Fast headless checks

Core tests need a C++23 compiler and CMake, without OpenGL development packages or
initialized submodules. They link the same `soso-core` library as the engine and
do not use its precompiled header. Each tested public header is included first in
its own test translation unit so missing direct includes break the build.

```bash
cmake -S . -B build/core-debug -DCMAKE_BUILD_TYPE=Debug -DSS_BUILD_ENGINE=OFF
cmake --build build/core-debug --parallel 2
ctest --test-dir build/core-debug --output-on-failure --no-tests=error
```

Use a separate `build/core-release` tree with `-DCMAKE_BUILD_TYPE=Release` for
optimized behavior. With a Visual Studio generator, select `--config Debug` at
build time and `-C Debug` for CTest; use Release in both places for that configuration.

Linux sanitizer checks (GCC or Clang):

```bash
cmake -S . -B build/core-sanitized -DCMAKE_BUILD_TYPE=Debug -DSS_BUILD_ENGINE=OFF -DSS_ENABLE_SANITIZERS=ON
cmake --build build/core-sanitized --parallel 2
ctest --test-dir build/core-sanitized --output-on-failure --no-tests=error
```

Choose compilers when creating a fresh build tree, for example with
`-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++`. Sanitizer options propagate
from `soso-core` to its consumers and fail on detected undefined behavior. A
ptrace-restricted sandbox may prevent LeakSanitizer from running; rerun in a
permitted environment rather than disabling leak checks or calling the run passed.

## Build and launch

First inspect `git status --short` and `git submodule status`. `.gitmodules` defines
submodule locations; the superproject pins their revisions. Initialize missing
submodules at those revisions without updating to remote branch tips. Preserve
existing local edits.

Linux vendored-library build:

```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug --parallel 2
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel 2
```

Adjust parallelism to available memory/CPU. Linux still needs a compiler with the
used C++23 library features, OpenGL development files, and GLFW's X11 development
prerequisites. The Linux CI workflow lists its distribution-specific packages.
Vendored Assimp/shaderc shared libraries are selected by default. `USE_SYSTEM_LIBS`
selects pkg-config packages on Linux, but current post-build copy commands still
refer to vendored paths; verify both linking and packaging when changing that mode.

Windows, using a Visual Studio multi-configuration generator/toolchain:

```powershell
cmake -S . -B build/windows
cmake --build build/windows --config Debug --parallel 2
cmake --build build/windows --config Release --parallel 2
```

On Linux, launch the output from its own directory so relative resources resolve:

```bash
cd build/debug/bin/Debug-Linux-x64/Sandbox
LD_LIBRARY_PATH=".:${LD_LIBRARY_PATH:-}" ./Sandbox
```

Alternatively, `scripts/build.sh Debug` and `scripts/run.sh Debug` use the existing
`build/` layout. They do not select the separate `build/debug/` directory above.
Windows output lives under `build/windows/bin/Debug-Windows-x64/Sandbox`; launch
with that directory as the working directory. Its current post-build step copies
`Resources` but not `assets`; verify asset staging before claiming Windows launch
works. Current resource-path behavior is a baseline limitation; deployment work
must remove source-tree/write assumptions.

## Select checks by behavior

| Change | Required evidence when applicable |
| --- | --- |
| Documentation or skills | Accurate paths/commands, resolved local links, valid skill frontmatter, review for contradictory instructions and unfinished scaffolds |
| Core logic, ownership, resources | Unit/regression tests, boundary and failure cases, sanitizer runs for exercised lifetime/memory paths |
| Simulation or gameplay | Controlled stepping/reset tests, frame-rate independence, bounded workload, relevant numerical invariants and tolerances |
| Rendering or shaders | Actual shader compile/link and resource bindings, GL debug output, representative scene comparison, resize/reload/failure checks |
| Optimization | Correctness checks plus repeatable before/after CPU/GPU frame-time and memory measurements on the same workload |
| Build, dependencies, packaging | Fresh configuration/build of affected configurations/platforms, dependency resolution, launch from packaged/install output |

Add regression tests with behavior changes using the existing doctest/CTest setup.
Keep suitable logic tests headless; graphics tests need a real compatible context,
not just mocked GL calls. Inspect registration when adding or moving tests:

```bash
ctest --test-dir build/debug -N
```

Execute tests with a command that fails when none are found:

```bash
ctest --test-dir build/debug --output-on-failure --no-tests=error
```

For a Windows multi-configuration tree, also select `-C Debug` or `-C Release`.
The doctest executable reports individual case/assertion counts; CTest reports one
entry for the complete core suite.

Use AddressSanitizer and UndefinedBehaviorSanitizer for appropriate supported C++
checks; use ThreadSanitizer separately when concurrency changes warrant it. Wire
instrumentation through compiler/link options on affected targets and dependencies
as needed; do not claim coverage for code or platforms that were not instrumented.

For graphics/performance checks, record CPU, GPU, driver, build, resolution, scene,
settings, VSync/frame cap, warm-up, sampling method, and sample count. Compare frame
times and spikes as well as throughput. Collect GPU timings without introducing
synchronous query stalls. Define workload-specific budgets rather than a universal
FPS promise. Software rendering is useful for supported smoke tests but does not
establish target-hardware performance or full driver compatibility.

## Review and reporting

Before a commit, inspect `git diff --check`, the complete task diff, and then
`git diff --cached --check` and the staged diff. Check new/untracked task files
explicitly: an ordinary `git diff` omits them. Preserve unrelated work and stage
only intended paths. Use the repository's change-review skill for the full review.

Report checks as passed, failed, or not run, with commands and material reasons.
Distinguish an existing baseline failure from a new regression using a baseline
comparison where feasible. A successful compilation, empty test run, or generated
screenshot is not evidence of checks that were never performed. Deployment claims
require an actual package/installation launch check on the supported environment.
