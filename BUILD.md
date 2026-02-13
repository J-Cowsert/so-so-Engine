# Build Instructions

## Linux (CMake)

### Prerequisites
All dependencies are vendored in the `so-so/vendor/` directory. No system packages required.

### Build Commands

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build -j$(nproc)

# Run
./run.sh
```

### Build Configurations
- **Debug**: Development build with debug symbols
- **Release**: Optimized build
- **Dist**: Distribution build (same as Release)

To change configuration:
```bash
./build.sh Release
./run.sh Release
```

Or manually:
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

### Clean Build
```bash
rm -rf build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

## Project Structure

- `so-so/` - Engine library (static)
- `Sandbox/` - Test application (executable)
- `so-so/vendor/` - All vendored dependencies
  - `GLFW` - Window/input handling
  - `Glad` - OpenGL loader
  - `imgui` - UI library
  - `tracy` - Profiling
  - `assimp` - Model loading
  - `shaderc` - Shader compilation
  - `spirv-cross` - Shader reflection
  - `glm` - Math library
  - `spdlog` - Logging

## Migration from Premake

The project has been migrated from Premake5 to CMake for Linux builds. Key changes:

1. **Vendored Dependencies**: All dependencies are now in `so-so/vendor/` (including shaderc for Linux)
2. **Build System**: Uses CMake instead of Premake
3. **Platform Defines**: `SS_PLATFORM_LINUX` is automatically defined on Linux builds
4. **Shared Libraries**: Assimp and shaderc are automatically copied to the output directory

## Notes

- The build uses C++23 standard
- Tracy profiling is enabled with on-demand mode
- OpenGLShader compilation is now enabled (was previously disabled with `#if 0`)
