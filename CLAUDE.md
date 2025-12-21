# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Runa2 is a C++20 SDL3 application for game/graphics development on Windows. The project uses CMake with Ninja and GCC/MinGW toolchain.

## Build System

This project uses **CMake** with **Ninja** as the generator and **GCC/MinGW** as the compiler on Windows.

### Build Commands

Configure and build Debug version:
```bash
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/debug
cmake --build build/debug
```

Configure and build Release version:
```bash
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/release
cmake --build build/release
```

Run the executable:
```bash
# Debug
./build/debug/Runa2.exe

# Release
./build/release/Runa2.exe
```

Clean build directories:
```bash
rm -rf build/debug
rm -rf build/release
```

## VSCode Integration

The project is fully configured for VSCode with one-command build, run, and debug workflows:

### Quick Actions

- **F5** - Build and debug (uses Debug configuration, automatically builds before debugging)
- **Ctrl+Shift+B** - Build Debug version (default build task)
- **Ctrl+Shift+P** → "Tasks: Run Task" → "Build and Run (Debug/Release)" - Build and run without debugging

### Available Tasks

Configured in [.vscode/tasks.json](.vscode/tasks.json):
- **Configure CMake (Debug/Release)** - Configure the build system
- **Build (Debug/Release)** - Compile the project
- **Build and Run (Debug/Release)** - Build and execute in one command
- **Clean (Debug/Release)** - Remove build directories

### Debug Configurations

Configured in [.vscode/launch.json](.vscode/launch.json):
- **Debug (Build and Run)** - Automatically builds before debugging
- **Debug (Run Only)** - Debug without rebuilding
- **Release (Build and Run)** - Build and run optimized version

All configurations use GDB as the debugger.

## Project Architecture

### Compiler Configuration

The project explicitly uses GCC/G++ from MinGW (installed via Scoop):
- **C++ Standard**: C++20 (required, no extensions)
- **Debug flags**: `-g -O0` for full debugging symbols and no optimization
- **Release flags**: `-O3` for maximum optimization
- **Toolchain**: Native GCC/MinGW from system PATH

A legacy toolchain file exists at [toolchains/clang-mingw.cmake](toolchains/clang-mingw.cmake) but is not currently used by the build system.

### Dependencies

- **SDL3**: Automatically fetched and built via CMake FetchContent from GitHub (main branch)
  - First build will be slower due to SDL3 compilation
  - Subsequent builds are incremental and much faster
- **nlohmann/json**: Included in `vendor/JSON/` directory (not currently used in main.cpp)

### Directory Structure

- `src/` - Application source code
  - `main.cpp` - Entry point with SDL3 window creation and event loop
- `Resources/` - Game assets (fonts, sprites, textures)
  - `Fonts/` - Font files
  - `mystic_woods_2.2/` - Game asset pack
- `build/` - Build output directories (gitignored)
  - `build/debug/` - Debug builds with symbols (`-g -O0`)
  - `build/release/` - Optimized release builds (`-O3`)
  - `build/debug/compile_commands.json` - For IntelliSense/clangd
- `vendor/` - Third-party libraries (currently only nlohmann/json)
- `.vscode/` - VSCode configuration for tasks, launch, and settings
- `toolchains/` - Unused legacy toolchain files

### VSCode Settings

[.vscode/settings.json](.vscode/settings.json) configures:
- CMake to use Ninja generator with GCC
- Default build type as Debug
- Clangd language server to use compile commands from `build/debug/`
- C/C++ extension to use CMake Tools for configuration

## Development Workflow

1. Open project in VSCode
2. Press **F5** to build and debug, or **Ctrl+Shift+B** to just build
3. Modify code - IntelliSense works via compile_commands.json
4. For release builds, use "Tasks: Run Task" → "Build and Run (Release)"

The configuration auto-generates compile commands on every CMake configure, ensuring clangd and C/C++ IntelliSense stay synchronized with the build.

## Important Notes

- The first build will take significantly longer (5-15 minutes) as CMake downloads and compiles SDL3 from source
- Ensure Git is installed and accessible in PATH for SDL3 FetchContent to work
- If SDL3 download fails, check internet connection and try: `rm -rf build` to clear cache
