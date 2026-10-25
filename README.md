# so-so Engine

A rendering engine built for exploring graphics programming and engine architecture.

![Sponza scene rendered with PBR, IBL, and shadow mapping](docs/soso_demo.jpg)

### Features
- PBR materials with metallic/roughness workflow
- Image-based lighting (IBL) with irradiance and radiance maps
- HDR environment maps with equirectangular-to-cubemap conversion
- SPIR-V shader compilation and reflection
- Shadow mapping (WIP)
- Post-processing pipeline (WIP)
- Debug rendering (lines, AABBs, wireframe)
- ImGui editor overlay
- Profiler integration

### Tech
- C++23, OpenGL 4.6
- CMake build system (Linux + Windows)
- All dependencies vendored

## Build

### Linux

```bash
scripts/build.sh          # Build (Debug by default)
scripts/build.sh Release  # Build Release
scripts/run.sh            # Run
```

### Windows

```bash
cmake -B build -S .
cmake --build build --config Debug
```

Or open `CMakeLists.txt` directly in Visual Studio (File > Open > CMake...)

## Development

Read [AGENTS.md](AGENTS.md) for project priorities, C++ standards, and repository
skills. See [development validation](docs/development-validation.md) for current
build commands, verification requirements, and known infrastructure gaps.

Core unit tests can be built without graphics dependencies or submodules:

```bash
cmake -S . -B build/core-debug -DCMAKE_BUILD_TYPE=Debug -DSS_BUILD_ENGINE=OFF
cmake --build build/core-debug --parallel 2
ctest --test-dir build/core-debug --output-on-failure --no-tests=error
```

## Resources
- [The Cherno](https://www.youtube.com/@TheCherno)
- [Learn OpenGL](https://learnopengl.com/)
- [Real Time Rendering](https://www.realtimerendering.com/)
- [Game Engine Architecture - Jason Gregory](https://www.gameenginebook.com/)
