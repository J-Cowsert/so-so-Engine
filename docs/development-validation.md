# Development validation

Run commands from the repository root. `CMakeLists.txt` and the current workflows
are authoritative when older build instructions disagree. Use separate build
directories for different configurations/toolchains; do not destroy another
developer's build tree to obtain a clean result.

## Current baseline

The project defines a C++23 static library, `so-so`, and a client executable,
`Sandbox`. CMake 3.25 or newer is required. Linux and Windows have Debug and
Release CI jobs; Dist is also declared by CMake. CI currently builds and uploads
artifacts; it does not run a project test suite or graphics acceptance checks.

At the initial guidance revision:

- The local Linux Debug engine library compiled, but Sandbox failed because
  `so-so/src/so-so/Core/UID.h` uses `uint64_t` without including `<cstdint>`.
  This is an observed failure, not a complete inventory of build defects.
- No first-party tests are registered with CTest. `TestLayer` is an interactive
  demo, not an automated test suite. A zero-test run does not validate behavior.
- Tracy is linked, but `SS_ENABLE_PROFILING` in `Core/Profiler.h` is `0`.
  Verify instrumentation is active before interpreting a profiling capture.
- `BUILD.md` contains obsolete script paths and overstates the absence of system
  prerequisites. The executable scripts are `scripts/build.sh` and `scripts/run.sh`.

Recheck these observations before relying on them and update this section as the
gaps are repaired. Do not mark unrelated work as tested because a known failure
exists, and do not silently include baseline repairs in a documentation change.

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

Add unit tests with the first behavior change that needs them, using a single
maintainable C++ test framework and CTest integration. Keep suitable logic tests
headless; graphics tests need a real compatible context, not just mocked GL calls.
Until tests are registered, this discovery command reports the gap:

```bash
ctest --test-dir build/debug -N
```

After integrating tests, use a command that fails when none are found:

```bash
ctest --test-dir build/debug --output-on-failure --no-tests=error
```

For a Windows multi-configuration tree, also select `-C Debug` or `-C Release`.
Do not present the future command as evidence that today's repository has tests.

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
