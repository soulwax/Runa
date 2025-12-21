# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

### Build Directory Structure

- `build/debug/` - Debug builds with symbols (`-g -O0`)
- `build/release/` - Optimized release builds (`-O3`)
- Compile commands database is generated at `build/debug/compile_commands.json` for IntelliSense

## Project Architecture

### Compiler Configuration

The project explicitly uses GCC/G++ from MinGW (installed via Scoop):
- **C++ Standard**: C++20 (required, no extensions)
- **Debug flags**: `-g -O0` for full debugging symbols and no optimization
- **Release flags**: `-O3` for maximum optimization
- **Toolchain**: Native GCC/MinGW from system PATH

A legacy toolchain file exists at [toolchains/clang-mingw.cmake](toolchains/clang-mingw.cmake) but is not currently used by the build system.

### VSCode Settings

[.vscode/settings.json](.vscode/settings.json) configures:
- CMake to use Ninja generator with GCC
- Default build type as Debug
- Clangd language server to use compile commands from `build/debug/`
- C/C++ extension to use CMake Tools for configuration

### Source Structure

- [src/main.cpp](src/main.cpp) - Entry point
- `build/` - Build output directories (gitignored)
- `.vscode/` - VSCode configuration for tasks, launch, and settings

## Development Workflow

1. Open project in VSCode
2. Press **F5** to build and debug, or **Ctrl+Shift+B** to just build
3. Modify code - IntelliSense works via compile_commands.json
4. For release builds, use "Tasks: Run Task" → "Build and Run (Release)"

The configuration auto-generates compile commands on every CMake configure, ensuring clangd and C/C++ IntelliSense stay synchronized with the build.
