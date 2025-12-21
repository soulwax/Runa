# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This project uses **CMake** with **Ninja** as the generator and **Clang/LLVM** as the compiler targeting Windows via MinGW.

### Build Commands

Configure the project:
```bash
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=toolchains/clang-mingw.cmake -B build
```

Build the project:
```bash
cmake --build build
```

Run the executable:
```bash
./build/Runa2.exe
```

Clean and reconfigure:
```bash
rm -rf build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=toolchains/clang-mingw.cmake -B build
```

## Project Architecture

### Toolchain Configuration

The project uses a custom CMake toolchain configuration defined in [CMakeLists.txt](CMakeLists.txt):
- **Compiler**: Clang/Clang++ targeting `x86_64-w64-mingw32`
- **Linker**: LLD (LLVM linker) via `-fuse-ld=lld`
- **Target**: Windows (MinGW environment)

This setup allows using modern LLVM tools while building for Windows.

### VSCode Integration

The [.vscode/settings.json](.vscode/settings.json) is configured for:
- CMake with Ninja generator
- Automatic toolchain file specification
- Clangd language server with compile commands from `build/compile_commands.json`

The compile commands database is automatically exported during CMake configuration, enabling accurate IntelliSense and code navigation.

### Source Structure

Currently minimal structure:
- [src/main.cpp](src/main.cpp) - Entry point
- `toolchains/` - Contains (or will contain) CMake toolchain files
- `build/` - Build output directory (generated, not in version control)

## Important Notes

- The toolchain is configured at the top level [CMakeLists.txt](CMakeLists.txt), though there's a `toolchains/` directory that appears to be intended for separate toolchain files
- The project uses Windows-specific tooling (MinGW) but with modern LLVM/Clang compiler infrastructure
- Clangd expects compile commands in the `build/` directory - ensure CMake has been configured before expecting full IDE features