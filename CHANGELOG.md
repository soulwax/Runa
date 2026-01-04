# Changelog

All notable changes to the Runa2 game engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-01-03

### Added
- **Vulkan2D Rendering Backend**: Migrated from SDL3 GPU API to Vulkan2D for hardware-accelerated 2D rendering
  - VK2DRenderer integration with SDL3 window system
  - SpriteBatch using Vulkan2D automatic batching for optimal performance
  - Texture loading via Vulkan2D with SDL3_image support
  - Runs at 59-61 FPS on NVIDIA GeForce RTX 3070 Ti
- **ECS Architecture**: Entity Component System using EnTT v3.13.2
  - Core components: Position, Velocity, Size, PlayerInput, Player tag
  - ECS systems: updatePlayerInput, updateMovement, updateAnimation, updateTileCollisions
  - EntityRegistry wrapper with helper functions for entity creation
- **Player Movement**: Fully functional player entity with keyboard controls
  - Arrow keys and WASD input support
  - 200 pixels/second movement speed
  - ECS-based input and physics systems
- **Grass Tile Rendering**: Working sprite sheet rendering demo
  - decor-grass.png (64x80 sprite sheet) for grass tiles
  - dirt-grass.png (64x64 sprite sheet) for dirt paths
  - 3,600 tiles (80x45 grid) rendered at 16x16 pixels each
  - Successful dirt path cross pattern rendering
- **Demo Applications**:
  - `grass_test.cpp`: Texture rendering test with grass tiles and dirt paths
  - `main.cpp`: ECS-based game with player movement on grass field

### Fixed
- **Critical Rendering Bug**: Fixed double-calling of `beginFrame()`/`endFrame()`
  - Application.cpp already calls these in mainLoop() at lines 113-115
  - Removed duplicate calls from game onRender() methods
  - Prevents silent crashes and window destruction on startup
- **Precompiled Header**: Fixed spdlog include order in runapch.h
  - Moved `#include <spdlog/spdlog.h>` before sink headers
  - Resolved IntelliSense error: "'spdlog/spdlog.h' file not found"
  - Proper order: spdlog.h → basic_file_sink.h → stdout_color_sinks.h
- **Texture Loading**: Fixed resource path handling
  - Use relative paths from project root (not absolute paths with leading slash)
  - Working directory correctly set to project root for Resource/ access

### Changed
- **CMakeLists.txt**: Updated build configuration
  - Changed default executable from grass_test.cpp to main.cpp
  - ECS-based game now builds by default
  - grass_test.cpp available as disabled alternative
- **CLAUDE.md**: Comprehensive documentation update
  - Added Vulkan2D and EnTT to dependencies section
  - Updated Graphics Pipeline documentation from SDL3 GPU to Vulkan2D
  - Added complete ECS architecture documentation with components and systems
  - Added proper rendering flow examples with beginFrame()/endFrame()
  - Added Git submodule initialization instructions for Vulkan2D
  - Documented DLL architecture and build system

### Technical Details
- **Renderer Architecture**:
  - Vulkan2D provides hardware-accelerated 2D rendering via Vulkan abstraction
  - Automatic sprite batching for optimal draw call performance
  - NEAREST filtering, VSYNC enabled, MSAA disabled
  - SwapChain with 3 images for triple buffering
- **Build System**:
  - CMake with Ninja generator
  - GCC/MinGW toolchain on Windows
  - Debug builds: `-g -O0` with full symbols
  - Release builds: `-O3` optimization
  - First build takes 5-15 minutes (SDL3, Vulkan2D, dependencies)
  - Incremental builds are fast
- **Dependencies**:
  - SDL3 (main branch) - Window and event handling
  - SDL3_image (main branch) - PNG/JPG texture loading
  - SDL3_ttf (main branch) - TrueType font rendering
  - Vulkan2D (Git submodule) - 2D Vulkan renderer backend
  - EnTT v3.13.2 - Entity Component System
  - spdlog v1.15.0 - Fast C++ logging
  - yaml-cpp (master) - YAML resource manifests

### Known Issues
- ResourceManager YAML sprite sheet loading causes silent crashes
  - Complex YAML files (decor-grass.yaml, dirt-grass.yaml) crash on loadTilesetFromAtlasYAML()
  - Workaround: Use direct Texture loading for now
  - Investigation needed for YAML parsing issue
- Plains tileset missing (plains.yaml/plains.png)
  - Original demo required non-existent Resources/mystic_woods_2.2/sprites/tilesets/plains.yaml
  - Using decor-grass/dirt-grass sprite sheets as replacement

### Performance
- **FPS**: Consistent 59-61 FPS (VSYNC-limited)
- **Tile Rendering**: 3,600 tiles per frame with no performance impact
- **Resolution**: 1280x720 window
- **GPU**: NVIDIA GeForce RTX 3070 Ti, Vulkan 1.4.325
- **CPU**: 16 logical cores, 31.93GB RAM
- **OS**: Windows (SDL 3.5.0)

### Controls
- **Arrow Keys** or **WASD** - Move player
- **ESC** - Quit application

---

## [Unreleased]

### Planned
- Fix ResourceManager YAML loading crash
- Implement camera following system
- Add collision detection with walls/obstacles
- Add enemy entities
- Add sprite animation system
- Implement tile-based collision maps
- Add sound system
- Add particle effects

---

[0.1.0]: https://github.com/yourusername/Runa2/releases/tag/v0.1.0
