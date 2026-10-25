# Context-free core and offline unit tests

## Context

The first automated regression tests cover UID value/hash behavior, layer-name
ownership, and resource equality. These operations do not need a window, graphics
context, asset importer, editor, or logger. Previously their headers pulled in
logging/graphics dependencies and the application build hid missing includes with
a precompiled header. The project had no CTest registration.

## Decision

Build `Layer.cpp` and `UID.cpp` once in the `soso-core` static library. Both the
OpenGL engine and tests link that production target. Its public headers use direct
standard-library includes and forward declarations where sufficient. Its source
files do not use the engine PCH. Preserve public behavior except for the documented
ownership/comparison defects being corrected.

`SS_BUILD_ENGINE=OFF` omits the graphics/dependency configuration entirely, allowing
core tests on machines without GPU development packages or initialized submodules.
The default remains the full engine/application build. Renderer/application rules
live in `cmake/Engine.cmake`; CTest and core configuration stay at the root.

Use the pinned, unmodified doctest single header only in the test target. Keep its
license and provenance beside it and require no configure-time network access.
CTest runs the complete core test executable with a timeout and a failing exit
status on assertion failure. New core/test code is compiled with strict warnings
treated as errors. An opt-in Linux GCC/Clang ASan+UBSan configuration instruments
the core and propagates instrumentation to its consumers.

## Alternatives and consequences

Linking tests to the complete renderer would run without a window but still
require its graphics packages, submodules, and shared libraries. Compiling separate
copies of production implementations in tests would let build settings diverge.
The shared core target avoids both. This is an initial boundary, not a wholesale
engine redesign: additional code moves into it only when its dependencies and
ownership fit.

Other established C++ frameworks could cover these tests. A single vendored header
keeps this initial setup small and offline; there is no runtime engine dependency.
Test discovery currently exposes one CTest entry containing the doctest cases.
Graphics, lifecycle integration, concurrency, and performance remain separate work
requiring appropriate tests and measurements; these unit tests do not certify them.
