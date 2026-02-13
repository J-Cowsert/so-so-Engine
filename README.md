# so-so Engine

> [!Note]
> **so-so** *(adjective)*: Neither very good nor very bad.

## Build

This project uses **CMake** as its build system.

### Linux

```bash
scripts/build.sh  # Build the project
scripts/run.sh    # Run Sandbox
```

Or manually:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
./build/bin/Debug-Linux-x64/Sandbox/Sandbox
```

### Windows

```bash
cmake -B build -S .
cmake --build build --config Debug
```

Or open `CMakeLists.txt` directly in Visual Studio (File → Open → CMake...)

### Build Configurations
- `Debug` - Development build with debug symbols
- `Release` - Optimized build
- `Dist` - Distribution build

## Resources
* [The Cherno](https://www.youtube.com/@TheCherno)
* [Learn OpenGL](https://learnopengl.com/)
* [Real Time Rendering](https://www.realtimerendering.com/)
* [Game Engine Architecture - Jason Gregory](https://www.gameenginebook.com/)
