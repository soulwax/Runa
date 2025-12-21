# Runa2

A C++20 project using SDL3 for window and graphics management, built with CMake, Ninja, and GCC/MinGW on Windows.

## Overview

Runa2 is a basic SDL3 application that creates a resizable window with an event loop. This serves as a foundation for game development or graphical applications.

## Prerequisites

Before building this project on Windows, you need the following tools installed:

### Required Tools

1. **CMake** (version 3.20 or higher)
   - Download from [cmake.org](https://cmake.org/download/)
   - Or install via Scoop: `scoop install cmake`

2. **Ninja Build System**
   - Download from [ninja-build.org](https://ninja-build.org/)
   - Or install via Scoop: `scoop install ninja`

3. **GCC/G++ (MinGW)**
   - Install via Scoop: `scoop install gcc`
   - Or download MinGW-w64 from [winlibs.com](https://winlibs.com/) or [mingw-w64.org](https://www.mingw-w64.org/)

4. **Git**
   - Required for fetching SDL3 dependency
   - Download from [git-scm.com](https://git-scm.com/)
   - Or install via Scoop: `scoop install git`

### Recommended: Scoop Package Manager

The easiest way to install all prerequisites on Windows:

```powershell
# Install Scoop (if not already installed)
irm get.scoop.sh | iex

# Install all required tools
scoop install cmake ninja gcc git
```

### Optional: VSCode

For the best development experience:

- **Visual Studio Code** with the following extensions:
  - C/C++ Extension Pack
  - CMake Tools
  - clangd (optional, for better IntelliSense)

## Building the Project

### Command Line Build

#### Debug Build

```bash
# Configure
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/debug

# Build
cmake --build build/debug

# Run
./build/debug/Runa2.exe
```

#### Release Build

```bash
# Configure
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/release

# Build
cmake --build build/release

# Run
./build/release/Runa2.exe
```

### VSCode Build

If you're using VSCode, the project includes pre-configured tasks and launch configurations:

1. **Open the project in VSCode**

   ```bash
   code .
   ```

2. **Build and Debug**
   - Press `F5` to build and start debugging (automatic build before debugging)

3. **Build Only**
   - Press `Ctrl+Shift+B` to build the Debug version

4. **Build and Run (without debugging)**
   - Press `Ctrl+Shift+P`
   - Type "Tasks: Run Task"
   - Select "Build and Run (Debug)" or "Build and Run (Release)"

## Project Structure

```sh
Runa2/
├── src/
│   └── main.cpp          # Entry point - SDL3 window creation and event loop
├── build/
│   ├── debug/            # Debug build output (gitignored)
│   └── release/          # Release build output (gitignored)
├── vendor/
│   └── JSON/             # JSON library (nlohmann/json)
├── .vscode/              # VSCode configuration
│   ├── tasks.json        # Build tasks
│   ├── launch.json       # Debug configurations
│   └── settings.json     # Editor settings
├── CMakeLists.txt        # CMake build configuration
├── CLAUDE.md             # AI assistant instructions
└── README.md             # This file
```

## Dependencies

Dependencies are automatically managed by CMake:

- **SDL3** - Fetched automatically from GitHub during CMake configuration
- **nlohmann/json** - Included in `vendor/JSON/` directory (currently not used in main.cpp)

The first build will take longer as CMake downloads and builds SDL3. Subsequent builds will be much faster.

## Compiler Configuration

- **C++ Standard**: C++20 (required, no extensions)
- **Compiler**: GCC/G++ (MinGW)
- **Debug Flags**: `-g -O0` (full debug symbols, no optimization)
- **Release Flags**: `-O3` (maximum optimization)
- **Build System**: CMake 3.20+ with Ninja generator

## Cleaning Build Files

To clean the build directories:

```bash
# Remove debug build
rm -rf build/debug

# Remove release build
rm -rf build/release

# Remove both
rm -rf build
```

Or use the VSCode tasks:

- `Ctrl+Shift+P` → "Tasks: Run Task" → "Clean (Debug)" or "Clean (Release)"

## Troubleshooting

### SDL3 Download Issues

If SDL3 fails to download:

- Check your internet connection
- Ensure Git is installed and in your PATH
- Try clearing the CMake cache: `rm -rf build`

### Compiler Not Found

If CMake can't find GCC:

- Verify GCC is installed: `gcc --version`
- Ensure GCC is in your PATH
- Try specifying the full path: `-DCMAKE_C_COMPILER="C:/path/to/gcc.exe"`

### Ninja Not Found

If CMake can't find Ninja:

- Verify Ninja is installed: `ninja --version`
- Ensure Ninja is in your PATH

### IntelliSense Not Working (VSCode)

The project generates `build/debug/compile_commands.json` for IntelliSense:

- Ensure you've run CMake configure at least once
- Check that clangd or C/C++ extension is using the compile commands database
- Try rebuilding: `cmake --build build/debug`

## Running the Application

When you run the executable, you should see:

1. A console window displaying "SDL3 initialized successfully!"
2. An 800x600 resizable window titled "Runa2 - SDL3 Test"

To exit:

- Close the window by clicking the X button
- Or press `Ctrl+C` in the console

## Development Workflow

1. **Make code changes** in `src/main.cpp` or other source files
2. **Build** using `F5` (VSCode) or `cmake --build build/debug`
3. **Debug** using F5 in VSCode (with breakpoints)
4. **Test** by running the executable

The project is configured for rapid iteration with:

- Incremental builds (only changed files recompile)
- Automatic compile commands generation for IntelliSense
- One-command build and debug in VSCode

## License

[Specify your license here]

## Contributing

[AGPLv3 License](LICENSE.md)

## Authors

soulwax@github
