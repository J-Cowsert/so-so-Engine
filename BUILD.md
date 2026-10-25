# Build and test

Use CMake 3.25+ and a compiler with the C++23 standard-library features used by the
engine. Linux and Windows are the current engine platforms. Dependencies are
vendored, but platform development packages/toolchains are still required.

From a fresh clone, initialize the submodule revisions pinned by the repository:

```bash
git submodule update --init --recursive
```

Preserve local changes when working in an existing checkout. Do not update
submodules to branch tips as part of an ordinary build.

## Linux

The default engine build uses the vendored Assimp/shaderc libraries. It still
requires OpenGL development files and GLFW's X11 development prerequisites. On
Ubuntu, the CI workflow installs `libgl1-mesa-dev` and `xorg-dev`; its system-library
build additionally installs `libassimp-dev` and `libshaderc-dev`.

```bash
scripts/build.sh Debug
ctest --test-dir build --output-on-failure --no-tests=error
scripts/run.sh Debug
```

`scripts/build.sh Release` builds Release in the same tree. Use separate build
directories for side-by-side configurations or different toolchains; see the
[validation guide](docs/development-validation.md#build-and-launch).

## Windows

Use a Visual Studio toolchain with the required C++23 library support:

```powershell
cmake -S . -B build/windows
cmake --build build/windows --config Debug --parallel 2
ctest --test-dir build/windows -C Debug --output-on-failure --no-tests=error
```

Select Release in both commands for an optimized build. The application is under
`build/windows/bin/Debug-Windows-x64/Sandbox`. Current asset-copy and working-directory
limitations are described in the validation guide; building alone does not verify
an installed/deployed application.

## Headless core tests

These need no OpenGL packages or initialized submodules. The test framework is
vendored and used only by the test executable.

```bash
cmake -S . -B build/core-debug -DCMAKE_BUILD_TYPE=Debug -DSS_BUILD_ENGINE=OFF
cmake --build build/core-debug --parallel 2
ctest --test-dir build/core-debug --output-on-failure --no-tests=error
```

For multi-configuration generators, add `--config Debug` to the build and `-C Debug`
to CTest. Use a separate tree with `-DSS_ENABLE_SANITIZERS=ON` for Linux GCC/Clang
ASan+UBSan checks. `BUILD_TESTING=OFF` omits the test executable and framework.

See [development validation](docs/development-validation.md) for supported commands,
checks by change type, current coverage, sanitizer constraints, and known gaps.
