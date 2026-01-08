# Runa2

The repository is called Runa, but the project is called Runa2. Why? Because my first attempt at creating a solid vulkan backend was not very successful.

For the sake of simplicity, this is "Runa2".

A cross-platform C++20 game engine using SDL3 and Vulkan2D for graphics, built with CMake, Ninja, and GCC.

## Overview

Runa2 is a cross-platform 2D game engine built on SDL3 and Vulkan2D. It features an Entity Component System (ECS), scene management, resource loading, input handling, and hardware-accelerated rendering.

## Prerequisites

Before building this project, you need the following tools installed:

### Required Tools

1. **CMake** (version 3.20 or higher)
   - Download from [cmake.org](https://cmake.org/download/)
   - Or use your package manager (see platform-specific instructions below)

2. **Ninja Build System**
   - Download from [ninja-build.org](https://ninja-build.org/)
   - Or use your package manager

3. **GCC/G++** (C++20 compatible)
   - Linux: Usually pre-installed, or install via package manager
   - macOS: Install via Homebrew or use Clang
   - Windows: Install MinGW-w64 from [winlibs.com](https://winlibs.com/)

4. **Git**
   - Required for fetching dependencies via CMake FetchContent
   - Install via your system's package manager

5. **Vulkan SDK** (usually included with GPU drivers)
   - Download from [vulkan.lunarg.com](https://vulkan.lunarg.com/) if needed
   - Most modern systems have Vulkan support built-in

### Platform-Specific Installation

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install cmake ninja-build g++ git vulkan-tools
```

#### macOS (Homebrew)

```bash
brew install cmake ninja gcc git
# Vulkan support via MoltenVK (included with Vulkan SDK or install separately)
brew install --cask vulkan-sdk
```

#### Windows (Scoop)

```powershell
# Install Scoop (if not already installed)
irm get.scoop.sh | iex

# Install all required tools
scoop install cmake ninja gcc git
```

#### Windows (Manual)

- CMake: Download from [cmake.org](https://cmake.org/download/)
- Ninja: Download from [ninja-build.org](https://ninja-build.org/)
- GCC: Download MinGW-w64 from [winlibs.com](https://winlibs.com/)
- Git: Download from [git-scm.com](https://git-scm.com/)

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
./build/debug/Runa2        # Linux/macOS
# or
./build/debug/Runa2.exe    # Windows
```

#### Release Build

```bash
# Configure
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/release

# Build
cmake --build build/release

# Run
./build/release/Runa2      # Linux/macOS
# or
./build/release/Runa2.exe  # Windows
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
├── src/                  # Engine code (Runa2Engine DLL)
│   ├── Core/             # Core engine systems
│   ├── Graphics/          # Rendering systems
│   ├── ECS/              # Entity Component System
│   ├── Scenes/           # Scene base classes
│   └── Vulkan/           # Vulkan2D renderer
├── Sandbox/              # Game implementation (uses engine)
│   ├── main_rpg.cpp      # RPG demo (PRIMARY)
│   ├── main.cpp          # Basic ECS demo
│   ├── main_input_demo.cpp
│   └── main_scene_demo.cpp
├── build/
│   ├── debug/            # Debug build output (gitignored)
│   └── release/          # Release build output (gitignored)
├── Resources/            # Game assets
├── .vscode/              # VSCode configuration
│   ├── tasks.json        # Build tasks
│   ├── launch.json       # Debug configurations
│   └── settings.json     # Editor settings
├── CMakeLists.txt        # CMake build configuration
└── README.md             # This file
```

## Dependencies

All dependencies are automatically fetched and built by CMake via FetchContent:

- **SDL3** - Core windowing, events, and input
- **SDL3_image** - Image loading (PNG, JPG, etc.)
- **SDL3_ttf** - TrueType font rendering
- **Vulkan2D** - 2D renderer using Vulkan (integrated in `src/Vulkan/`)
- **EnTT** - Entity Component System library (v3.13.2)
- **yaml-cpp** - YAML parsing for resource manifests
- **spdlog** - Fast C++ logging library (v1.15.0)
- **nlohmann/json** - JSON library

The first build will take 5-15 minutes as CMake downloads and compiles all dependencies. Subsequent builds are incremental and much faster.

## Compiler Configuration

- **C++ Standard**: C++20 (required, no extensions)
- **Compiler**: GCC/G++ (or Clang on macOS)
- **Debug Flags**: `-g -O0` (full debug symbols, no optimization)
- **Release Flags**: `-O3` (maximum optimization)
- **Build System**: CMake 3.20+ with Ninja generator
- **Architecture**: Shared library engine + executable game

The project uses a shared library architecture:

- `Runa2Engine` - Shared library containing all engine code
- `Runa2` - Game executable that links to the engine

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

When you run the executable, you'll see the current demo (RPG demo by default) which includes:

- Player character with WASD movement
- Tile-based world with collisions
- Camera following the player
- ECS-based entity management

To exit:

- Close the window by clicking the X button
- Or press ESC (depending on the scene)

## Development Workflow

1. **Make code changes**:
   - Engine code: `src/` directory (Runa2Engine DLL)
   - Game code: `Sandbox/` directory (game implementation)
2. **Build** using `F5` (VSCode) or `cmake --build build/debug`
3. **Debug** using F5 in VSCode (with breakpoints)
4. **Test** by running the executable

The project is configured for rapid iteration with:

- Incremental builds (only changed files recompile)
- Automatic compile commands generation for IntelliSense
- One-command build and debug in VSCode

## License

[AGPLv3 License](LICENSE.md)

## Authors

soulwax@github
