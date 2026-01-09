# Changelog

All notable changes to the Runa2 game engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.4] - 2026-01-09

### Added
- **Meadow Rendering System**: Implemented layered tile rendering for game environments
  - Base grass layer using `grass.png` (no transparency) - fills entire meadow
  - Decorative grass layer using `decor-grass.png` (with transparency) - 20% coverage for visual variety
  - Decorative objects layer using `objects.png` (with transparency) - 1% coverage for scattered items
  - Proper layering ensures transparent tiles render above solid base tiles
- **Game Implementation**: Created `Sandbox/main_game.cpp` as clean game entry point
  - Blank canvas with dark blueish sky background
  - FPS counter display (updates once per second, white text)
  - Meadow background rendering system
  - Foundation for future game features

### Changed
- **Sprite Rendering**: Updated coordinate system handling in `renderSprites()`
  - Fixed sprite sheet rendering to correctly adjust from center coordinates to top-left
  - Ensures sprites are properly centered on entities
  - Colored rectangle rendering also uses correct coordinate adjustment
- **Meadow Generation**: Improved meadow tile distribution
  - Increased decorative grass coverage from 5% to 20% for richer visuals
  - Added sparse object placement (1% coverage) for environmental variety
  - Only uses non-transparent grass tiles for base layer
  - Only uses regular decorative grass tiles (0-15), skips mud tiles (16-19)

### Fixed
- **Sprite Coordinate Alignment**: Fixed sprite rendering misalignment bug
  - `SpriteBatch::draw()` expects top-left coordinates, but `worldToScreen()` returns center coordinates
  - Adjusted sprite rendering to subtract half frame width/height before drawing
  - Ensures sprite sheets are correctly centered on entity positions

---

## [1.0.3] - 2026-01-08

### Added
- **Codebase Separation**: Separated engine code from game implementation
  - Created `Sandbox/` directory for game implementations
  - Engine code (all `Runa::` namespace) remains in `src/` directory
  - Game entry points moved: `main.cpp`, `main_rpg.cpp`, `main_input_demo.cpp`, `main_scene_demo.cpp` → `Sandbox/`
  - Clear separation between reusable engine DLL and application-specific game code
  - Added `Sandbox/README.md` documenting the separation

### Changed
- **Project Structure**: Reorganized codebase for better maintainability
  - Engine code (`src/`) compiles into `Runa2Engine` shared library (DLL)
  - Game code (`Sandbox/`) compiles into `Runa2` executable that links to engine DLL
  - Updated CMakeLists.txt to build from `Sandbox/main_rpg.cpp`
  - Updated include paths: Sandbox files can access engine headers via `src/` include directory
- **Folder Rename**: Renamed `src/Vulkan2D/` to `src/Vulkan/`
  - Updated CMakeLists.txt to reference `src/Vulkan` instead of `src/Vulkan2D`
  - Updated documentation (README.md, REPOSITORY_ANALYSIS.md) with new folder path
  - Library name "Vulkan2D" remains unchanged (only folder path renamed)
- **Documentation Updates**: Updated project documentation to reflect new structure
  - README.md: Updated project structure section
  - REPOSITORY_ANALYSIS.md: Updated architecture overview and project structure
  - Added code organization section explaining engine vs game separation

### Fixed
- **Font Rendering Bug**: Fixed incorrect `TTF_RenderText_Blended` API usage
  - SDL3_ttf requires 4 parameters: `(font, text, length, color)`
  - Fixed calls in `Font.cpp` to include text length parameter
  - Resolves incomplete text rendering (e.g., "Ite-" fragments)
- **Entity Rendering**: Fixed entities without sprite sheets not rendering
  - Updated `renderSprites()` to render entities as colored rectangles when no sprite sheet is available
  - Uses white pixel texture scaled to entity size with tint colors
  - Entities now visible as colored rectangles (player: blue, enemies: green, items: red/yellow)
- **Tile Rendering**: Fixed tiles not being drawn in world
  - Implemented basic tile rendering in `renderWorld()` using colored rectangles
  - Tiles render based on tile type (grass: green, dirt: brown, rock: gray)
  - Camera culling properly calculates visible tile range
- **Camera Initialization**: Fixed camera not positioned at player start
  - Added camera position initialization in `onInit()` to match player starting position
  - Camera now correctly follows player from game start

### Removed
- **VulkanInterface Files**: Removed `VulkanInterface.h` and `VulkanInterface.c`
  - Files deleted from `src/Vulkan/VK2D/include/VK2D/` and `src/Vulkan/VK2D/src/`
  - Removed reference from `src/Vulkan/CMakeLists.txt` to prevent build errors

---

## [1.0.2] - 2026-01-08

### Fixed
- **CRITICAL: SDL Initialization Bug** - Fixed inverted logic in `SDL_Init()` check
  - Changed `if (!SDL_Init(...))` to `if (SDL_Init(...) != 0)` in `Application.cpp`
  - SDL was incorrectly throwing errors on successful initialization
  - Prevents application from failing to start
- **CRITICAL: TTF Initialization Bug** - Fixed inverted logic in `TTF_Init()` check
  - Changed `if (!TTF_Init())` to `if (TTF_Init() != 0)` in `Application.cpp`
  - Font system was incorrectly logging warnings on successful initialization
- **Input System Double-Clearing** - Removed redundant `beginFrame()` call
  - Removed `beginFrame()` call from `Window::processEvents()` (line 42)
  - `Application::mainLoop()` already calls `beginFrame()` once per frame
  - Prevents input "just pressed/released" states from being cleared twice
  - Fixes missed key presses and mouse button events
- **Window Move Operations** - Disabled move constructor/assignment
  - Window contains raw `Input*` pointer that could become dangling after move
  - Changed from `= default` to `= delete` for move operations
  - Prevents potential undefined behavior from moved Window objects
- **ResourceManager Memory Management** - Fixed SDL_GetBasePath() usage
  - SDL3's `SDL_GetBasePath()` returns const pointer to internally-managed memory
  - Removed incorrect `SDL_free()` call (SDL3 manages memory automatically)
  - Prevents potential double-free or use-after-free errors
- **Camera Division by Zero Protection** - Added safety checks in `setZoom()`
  - Clamps zoom values to prevent division by zero in coordinate transforms
  - Minimum zoom: 0.25f, Maximum zoom: 4.0f
  - Ensures `worldToScreen()` and `screenToWorld()` never divide by zero
- **EntityRegistry Entity Count** - Implemented `getEntityCount()` method
  - Previously always returned 0 (unimplemented)
  - Now correctly counts entities via `Active` component view
  - Uses EnTT view iteration since registry doesn't have direct `size()` method

### Changed
- **Debug Logging**: Throttled texture creation debug message to update once per second
  - `[debug] Created texture from pixels ...` now logs at most once per second
  - Reduces log spam when textures are created frequently
  - Implemented using `std::chrono::steady_clock` for accurate timing

---

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

[1.0.4]: https://github.com/soulwax/Runa/releases/tag/v1.0.4
[1.0.3]: https://github.com/soulwax/Runa/releases/tag/v1.0.3
[1.0.2]: https://github.com/soulwax/Runa/releases/tag/v1.0.2
[0.1.1]: https://github.com/soulwax/Runa/releases/tag/v0.1.1
[0.1.0]: https://github.com/soulwax/Runa/releases/tag/v0.1.0
