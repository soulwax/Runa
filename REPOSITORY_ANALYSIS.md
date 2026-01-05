# Runa2 Repository Analysis

**Generated:** 2025-01-03  
**Project Type:** C++20 2D Game Engine  
**License:** AGPLv3  
**Author:** soulwax@github

---

## Executive Summary

**Runa2** is a modern C++20 2D game engine built on SDL3's GPU API with Vulkan2D backend for hardware-accelerated rendering. The engine features a complete Entity-Component-System (ECS) architecture using EnTT, comprehensive resource management, sprite batching, tilemap rendering, camera systems, and collision detection. It uses a DLL-based architecture separating engine code from game logic, enabling rapid iteration and code reuse.

**Current Status:** Core infrastructure is complete and functional. The engine features a fully implemented ECS system, working rendering pipeline with Vulkan2D, input management, collision detection, and camera system. The project is actively developed with both ECS-based and legacy entity-based systems available.

---

## Project Overview

### Purpose
A professional-grade 2D game engine framework designed for:
- 2D sprite-based games (RPGs, platformers, top-down games)
- Tilemap-based level rendering
- Hardware-accelerated graphics (Vulkan via Vulkan2D)
- Rapid game development with declarative resource management
- Entity-Component-System architecture for scalable game logic

### Target Platform
- **Primary:** Windows (MinGW/GCC)
- **Graphics Backends:** Vulkan (via Vulkan2D abstraction)
- **Build System:** CMake 3.20+ with Ninja generator
- **C++ Standard:** C++20 (strict, no extensions)

---

## Technology Stack

### Core Technologies
- **Language:** C++20 (strict standard, no extensions)
- **Compiler:** GCC/G++ (MinGW) on Windows
- **Build System:** CMake 3.20+ with Ninja generator
- **Graphics API:** Vulkan2D (2D Vulkan renderer) with SDL3 window management
- **Shaders:** GLSL 450 compiled to SPIR-V (offline compilation)
- **ECS Framework:** EnTT v3.13.2 (header-only entity-component-system)

### Dependencies (via CMake FetchContent)

| Dependency | Version | Purpose | Status |
|------------|---------|---------|--------|
| **SDL3** | main branch | Window management, events | ✅ Fully integrated |
| **SDL3_image** | main branch | Image loading (PNG/JPG support) | ✅ Fully integrated |
| **SDL3_ttf** | main branch | TrueType font rendering | ✅ Fully integrated |
| **Vulkan2D** | Git submodule | 2D Vulkan renderer backend | ✅ Fully integrated |
| **yaml-cpp** | master branch | YAML parsing for resource manifests | ✅ Fully integrated |
| **spdlog** | v1.15.0 | Fast logging library | ✅ Fully integrated |
| **EnTT** | v3.13.2 | Entity-Component-System framework | ✅ Fully integrated |

### External Tools Required
- **Vulkan SDK** - For shader compilation (`glslc` compiler)
- **Git** - For CMake FetchContent dependency management

---

## Architecture

### Build Architecture: DLL-Based Design

The project uses a **modular DLL architecture** for separation of concerns:

```
┌─────────────────────────────────────┐
│  Runa2Engine (DLL)                 │
│  - Core systems                     │
│  - Graphics systems                 │
│  - ECS systems                      │
│  - Resource management              │
│  - Exports via RUNA_API macro      │
└─────────────────────────────────────┘
              ▲
              │ Links to
              │
┌─────────────────────────────────────┐
│  Runa2 (Executable)                 │
│  - GameApp class                    │
│  - Game-specific logic              │
│  - Links to Runa2Engine.dll        │
└─────────────────────────────────────┘
```

**Benefits:**
- Faster iteration (only game code recompiles for game changes)
- Clean API boundaries via `RUNA_API` export macro
- Potential for engine reuse across multiple games
- Separation of engine and game code

### Application Framework

The engine follows a classic game loop pattern:

```
Application (Core)
    ├── Window (Graphics) - SDL3 window & events
    ├── Renderer (Graphics) - Vulkan2D renderer
    ├── Input (Core) - Keyboard, mouse, gamepad input
    └── Game Loop
        ├── onInit() - One-time initialization
        ├── onUpdate(deltaTime) - Per-frame game logic
        ├── onRender() - Per-frame rendering
        └── onShutdown() - Cleanup
```

**Virtual Hooks Pattern:**
- Base `Application` class provides game loop infrastructure
- Derived classes (e.g., `GameApp`) override virtual methods
- Clean separation between engine and game logic

### Module Organization

#### 1. Core Module (`src/Core/`)

| Class | Purpose | Status |
|-------|---------|--------|
| **Application** | Main game loop, delta time, FPS tracking | ✅ Complete |
| **ResourceManager** | Centralized resource loading, YAML parsing | ✅ Complete |
| **Log** | spdlog wrapper (console + file logging) | ✅ Complete |
| **Input** | Keyboard, mouse, gamepad input management | ✅ Complete |
| **Entity** | Legacy entity class (pre-ECS) | ✅ Complete |
| **PlayerController** | Input-to-velocity conversion | ✅ Complete |
| **Collision** | AABB collision detection | ✅ Complete |

#### 2. Graphics Module (`src/Graphics/`)

| Class | Purpose | Status |
|-------|---------|--------|
| **Window** | SDL3 window creation, event processing | ✅ Complete |
| **Renderer** | Vulkan2D renderer, frame rendering | ✅ Complete |
| **Shader** | SPIR-V shader loading from compiled GLSL | ✅ Complete |
| **Texture** | Image loading via SDL3_image, GPU uploads | ✅ Complete |
| **SpriteSheet** | Texture atlas with sprite/animation metadata | ✅ Complete |
| **SpriteBatch** | 2D sprite batching system (Vulkan2D automatic batching) | ✅ Complete |
| **TileMap** | Grid-based tilemap rendering | ✅ Complete |
| **PostProcess** | Post-processing effects framework | 🚧 Partial |
| **Font** | TrueType font rendering | ✅ Complete |
| **Camera** | 2D camera with world-to-screen transforms | ✅ Complete |

#### 3. ECS Module (`src/ECS/`)

| Component/System | Purpose | Status |
|------------------|---------|--------|
| **Components.h** | Component definitions (Position, Velocity, Sprite, etc.) | ✅ Complete |
| **Registry.h/cpp** | EnTT registry wrapper with helper functions | ✅ Complete |
| **Systems.h/cpp** | ECS systems (input, physics, animation, collision, rendering) | ✅ Complete |

**ECS Components:**
- **Transform:** Position, Velocity, Size, Transform
- **Rendering:** Sprite, Animation
- **Collision:** AABB, CollisionLayer
- **Input:** PlayerInput, CameraTarget
- **Tags:** Player, Enemy, Projectile, Pickup, Static, Active

**ECS Systems:**
- `updatePlayerInput()` - Convert input to velocity
- `updateMovement()` - Apply velocity to position
- `updateAnimation()` - Update sprite animation frames
- `updateTileCollisions()` - Resolve tilemap collisions
- `updateEntityCollisions()` - Entity-to-entity collision detection
- `renderSprites()` - Render all sprites with camera transform
- `updateCameraFollow()` - Smooth camera following

---

## File Structure

```
Runa2/
├── src/
│   ├── main.cpp              # Entry point, GameApp class (ECS version)
│   ├── grass_test.cpp        # Texture rendering test (disabled)
│   ├── runapch.h/cpp         # Precompiled header (common includes)
│   ├── RunaAPI.h             # DLL export/import macros
│   │
│   ├── Core/
│   │   ├── Application.*     # Base application framework
│   │   ├── ResourceManager.* # Resource loading & management
│   │   ├── Log.*             # Logging wrapper (spdlog)
│   │   ├── Input.*           # Input management (keyboard, mouse)
│   │   ├── Entity.*          # Legacy entity class
│   │   ├── PlayerController.*# Input-to-velocity conversion
│   │   └── Collision.*       # AABB collision detection
│   │
│   ├── Graphics/
│   │   ├── Window.*          # SDL3 window management
│   │   ├── Renderer.*        # Vulkan2D renderer
│   │   ├── Shader.*          # SPIR-V shader loading
│   │   ├── Texture.*         # Image loading & GPU textures
│   │   ├── SpriteSheet.*     # Texture atlas & sprite metadata
│   │   ├── SpriteBatch.*     # 2D sprite batching
│   │   ├── TileMap.*         # Grid-based tilemap system
│   │   ├── PostProcess.*     # Post-processing effects
│   │   ├── Font.*            # TrueType font rendering
│   │   └── Camera.*          # 2D camera with transforms
│   │
│   ├── Vulkan2D/             # Vulkan2D submodule integration
│   │   └── VK2D/             # Vulkan2D headers and source
│   │
│   └── ECS/
│       ├── Components.h      # Component definitions
│       ├── Registry.*       # EnTT registry wrapper
│       └── Systems.*        # ECS system functions
│
├── Resources/
│   ├── Fonts/                # Font files (Renogare.otf/ttf)
│   ├── manifests/            # YAML resource definitions
│   │   └── resource_manifest.yaml
│   ├── shaders/              # GLSL shaders (30+ shader pairs)
│   │   ├── *.vert/frag       # Vertex/fragment shaders
│   │   ├── compiled/         # Compiled SPIR-V files
│   │   └── compile_shaders.*# Compilation scripts
│   └── SpiteSheets/          # Sprite sheet images and YAML
│       ├── *.png             # Sprite sheet images
│       ├── *.yaml            # Sprite definitions
│       └── characters/       # Character sprites
│
├── build/                    # Build output (gitignored)
│   └── debug/                # Debug builds with symbols
│
├── vendor/                   # Third-party libraries (fetched by CMake)
│   ├── SDL/                  # SDL3 source
│   ├── SDL_image/            # SDL3_image source
│   ├── SDL_ttf/              # SDL3_ttf source
│   ├── yaml-cpp/             # yaml-cpp source
│   ├── spdlog/               # spdlog source
│   └── EnTT/                 # EnTT source (header-only)
│
├── CMakeLists.txt            # Build configuration
├── README.md                 # Build instructions
├── ARCHITECTURE.md           # Engine architecture docs
├── REPOSITORY_ANALYSIS.md    # This file
├── RENDERING_STATUS.md       # Rendering system status
├── CHANGELOG.md              # Version history
└── CLAUDE.md                 # AI assistant instructions
```

---

## Key Systems

### 1. Entity-Component-System (ECS)

**Framework:** EnTT v3.13.2 (header-only, high-performance ECS)

**Architecture:**
- **Components:** Plain data structures (Position, Velocity, Sprite, etc.)
- **Systems:** Free functions that operate on component sets
- **Registry:** EnTT registry wrapped in `EntityRegistry` class

**Component Types:**
- **Transform Components:** Position, Velocity, Size, Transform
- **Rendering Components:** Sprite, Animation
- **Collision Components:** AABB, CollisionLayer
- **Input Components:** PlayerInput, CameraTarget
- **Tag Components:** Player, Enemy, Projectile, Pickup, Static, Active

**System Functions:**
- `updatePlayerInput()` - Reads input and sets velocities
- `updateMovement()` - Applies velocity to position
- `updateAnimation()` - Updates sprite animation frames
- `updateTileCollisions()` - Resolves collisions with tilemap
- `updateEntityCollisions()` - Entity-to-entity collision detection
- `renderSprites()` - Renders all sprites with camera transform
- `updateCameraFollow()` - Smooth camera following

**Usage Example:**
```cpp
// Create player entity
auto player = registry->createPlayer(x, y, spriteSheet, "sprite_name", 120.0f);

// In update loop
Runa::ECS::Systems::updatePlayerInput(registry, input, dt);
Runa::ECS::Systems::updateMovement(registry, dt);
Runa::ECS::Systems::updateTileCollisions(registry, *tileMap, 16);
Runa::ECS::Systems::renderSprites(registry, *spriteBatch, *camera);
```

### 2. Rendering Pipeline (Vulkan2D)

**Frame Rendering Flow:**
```
1. Renderer::beginFrame()     - Acquire swapchain texture
2. Renderer::clear()          - Set background color
3. SpriteBatch::begin()       - Start batching
4. SpriteBatch::draw()        - Queue draw calls
5. SpriteBatch::end()         - Flush to GPU (Vulkan2D automatic batching)
6. Renderer::endFrame()       - Submit and present
```

**Current Status:**
- ✅ Vulkan2D integration with SDL3 window
- ✅ Swapchain management
- ✅ Frame begin/end
- ✅ Clear operations
- ✅ Automatic sprite batching (Vulkan2D handles batching)
- ✅ Texture loading and binding
- ✅ Alpha blending
- ✅ Textured sprite rendering
- ✅ Performance: 59-61 FPS (VSYNC-limited) on RTX 3070 Ti

**Shader System:**
- GLSL 450 shaders compiled to SPIR-V offline
- 30+ shader effects available (bloom, crt, pixelate, water, etc.)
- Shader compilation via `glslc` (Vulkan SDK)

### 3. Resource Management System

**YAML-Based Manifest System:**
- Declarative sprite definitions in YAML files
- Supports multiple sprite types: `single`, `animation`, `grid`, `frames`
- Automatic sprite generation from grid layouts
- Centralized loading via `ResourceManager`

**Example Manifest:**
```yaml
tilesets:
  - name: "plains"
    yaml: "../mystic_woods_2.2/sprites/tilesets/plains.yaml"
    image: "../mystic_woods_2.2/sprites/tilesets/plains.png"
    format: "atlas_coords"
```

**Features:**
- ✅ YAML parsing via yaml-cpp
- ✅ Sprite metadata (frames, animations, durations)
- ✅ Texture loading via SDL3_image
- ✅ GPU texture uploads
- ✅ Sprite lookup by name
- ✅ Tileset loading from atlas YAML files

**Known Issues:**
- ⚠️ ResourceManager YAML sprite sheet loading causes silent crashes
- Workaround: Use direct Texture loading for now

### 4. Input System

**Features:**
- ✅ Keyboard input (key down, pressed, released)
- ✅ Mouse input (button states, position, wheel)
- ✅ Frame-based input tracking (pressed/released events)
- ✅ Integration with ECS player input system

**Usage:**
```cpp
if (input.isKeyDown(SDLK_w)) {
    // Move up while key held
}
if (input.isKeyPressed(SDLK_SPACE)) {
    // Jump on space press
}
```

### 5. Camera System

**Features:**
- ✅ World-to-screen coordinate transformation
- ✅ Screen-to-world coordinate transformation
- ✅ Smooth camera following (lerp-based)
- ✅ Manual camera controls (WASD, mouse drag, mouse wheel zoom)
- ✅ World bounds calculation for culling
- ✅ Zoom support

**Usage:**
```cpp
camera->setPosition(x, y);
camera->followEntity(registry, playerEntity, 0.15f);
camera->handleInput(input, dt, 300.0f);
camera->worldToScreen(worldX, worldY, screenX, screenY);
```

### 6. Collision System

**Features:**
- ✅ AABB (Axis-Aligned Bounding Box) collision detection
- ✅ Tilemap collision detection
- ✅ Entity-to-entity collision detection
- ✅ Collision layers and masks
- ✅ Integration with ECS system

**Components:**
- `AABB` - Collision box with offset
- `CollisionLayer` - Layer and mask for filtering

### 7. Logging System

**spdlog Integration:**
- ✅ Console logging with colored output
- ✅ File logging to `logs/runa2.log` with timestamps
- ✅ Multiple log levels (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL)
- ✅ Auto-flush on warnings and above
- ✅ Graceful fallback if file logging fails

**Log Patterns:**
- Console: `[%^%l%$] %v` (colored level, message)
- File: `[%Y-%m-%d %H:%M:%S.%e] [%l] %v` (timestamp, level, message)

### 8. TileMap System

**Features:**
- ✅ Grid-based tilemap (configurable dimensions)
- ✅ Text file scene loading
- ✅ Tile index management
- ✅ SpriteSheet integration
- ✅ Solid tile marking for collision
- ✅ Efficient rendering with camera culling

---

## Design Patterns

### Memory Management
- **RAII** - All resources managed via smart pointers
- **Unique pointers** - Owned resources (Window, Renderer, ResourceManager)
- **Shared pointers** - Shared resources (Shader)
- **Move semantics** - Enabled for all graphics classes
- **No copying** - Graphics classes are non-copyable (deleted copy constructors)

### Error Handling
- **Exceptions** - Used for initialization failures
- **Runtime errors** - Logged via spdlog
- **Null checks** - Resource lookups return pointers (nullptr on failure)
- **Graceful degradation** - Fallback patterns where appropriate

### Code Organization
- **Namespace:** All engine code in `Runa` namespace
- **Module separation:** Core vs Graphics vs ECS modules
- **Include conventions:** Relative includes within modules, absolute from main
- **Precompiled headers:** `runapch.h` for common includes

### API Design
- **DLL exports:** `RUNA_API` macro for Windows DLL exports
- **Virtual hooks:** Application class provides extensible game loop
- **Getter methods:** Protected accessors for Window, Renderer, Input
- **Const correctness:** Const methods where appropriate

---

## Build Configuration

### Compiler Flags
- **Debug:** `-g -O0` (full symbols, no optimization)
- **Release:** `-O3` (maximum optimization)
- **C++ Standard:** C++20 (required, no extensions)
- **Export compile commands:** Enabled for IntelliSense/clangd

### Build Process
1. CMake configures project and fetches dependencies
2. First build compiles SDL3, Vulkan2D, and dependencies from source (5-15 minutes)
3. Subsequent builds are incremental
4. Generates `compile_commands.json` for IntelliSense
5. DLLs automatically copied to output directory

### Development Tools
- **VSCode** - Fully configured with tasks and launch configs
- **F5** - Build and debug
- **Ctrl+Shift+B** - Build only
- **clangd** - Language server support via compile commands

---

## Implementation Status

### ✅ Fully Implemented

| System | Status | Notes |
|--------|--------|-------|
| **Application Framework** | ✅ Complete | Game loop, delta time, FPS tracking |
| **Window Management** | ✅ Complete | SDL3 window, events, resize handling |
| **Vulkan2D Renderer** | ✅ Complete | Vulkan backend, swapchain, automatic batching |
| **Texture Loading** | ✅ Complete | SDL3_image integration, GPU uploads |
| **SpriteSheet System** | ✅ Complete | YAML parsing, sprite metadata |
| **ResourceManager** | ✅ Complete | Centralized resource loading |
| **TileMap System** | ✅ Complete | Grid-based rendering, scene loading |
| **Shader Loading** | ✅ Complete | SPIR-V shader loading |
| **Logging System** | ✅ Complete | spdlog integration (console + file) |
| **Font Rendering** | ✅ Complete | TrueType font support |
| **SpriteBatch** | ✅ Complete | Vulkan2D automatic batching |
| **Input System** | ✅ Complete | Keyboard, mouse, gamepad input |
| **Camera System** | ✅ Complete | World-to-screen transforms, following |
| **Collision System** | ✅ Complete | AABB, tilemap, entity collisions |
| **ECS Framework** | ✅ Complete | EnTT integration, components, systems |
| **Player Controller** | ✅ Complete | Input-to-velocity conversion |

### 🚧 Partially Implemented

| System | Status | Notes |
|--------|--------|-------|
| **PostProcess** | 🚧 Partial | Framework exists, needs testing |
| **ResourceManager YAML** | 🚧 Partial | Complex YAML loading causes crashes, workaround available |

### ❌ Not Yet Implemented

| System | Status | Notes |
|--------|--------|-------|
| **Audio System** | ❌ Not started | No SDL3_audio integration |
| **Physics Engine** | ❌ Not started | No physics integration (beyond basic collision) |
| **Scene Graph** | ❌ Not started | No hierarchical scene management |
| **Scripting** | ❌ Not started | No Lua or similar scripting support |
| **Networking** | ❌ Not started | No multiplayer support |

---

## Notable Features

### Strengths
1. **Clean Architecture** - Well-separated modules (Core, Graphics, ECS)
2. **Modern C++** - Smart pointers, RAII, move semantics, C++20 features
3. **Extensible Design** - Virtual hooks in Application class, ECS architecture
4. **Comprehensive Logging** - spdlog integration throughout
5. **Declarative Resources** - YAML-based sprite definitions
6. **Professional Build System** - CMake with proper dependency management
7. **DLL Architecture** - Separation of engine and game code
8. **ECS Integration** - Full EnTT-based entity-component-system
9. **Complete Rendering** - Working sprite batching with Vulkan2D GPU pipeline
10. **Rich Shader Library** - 30+ shader effects available
11. **Documentation** - Well-documented architecture and workflows

### Areas for Improvement
1. **Post-Processing** - Needs completion and testing
2. **Audio System** - No audio support yet
3. **Physics Engine** - Only basic collision, no physics simulation
4. **Animation Controller** - Animation metadata exists but could be enhanced
5. **Asset Pipeline** - Could benefit from automated asset processing
6. **ResourceManager YAML** - Fix complex YAML loading crashes

---

## Development Workflow

### Quick Start
1. **Build:** `F5` in VSCode or `cmake --build build/debug`
2. **Run:** Execute `build/debug/Runa2.exe`
3. **Debug:** Use VSCode debugger with breakpoints
4. **Iterate:** Incremental builds for rapid development

### Resource Workflow
1. **Create YAML manifest** in `Resources/manifests/`
2. **Load in game** via `ResourceManager::loadSpriteSheetFromYAML()` (or direct Texture loading as workaround)
3. **Access sprites** via `ResourceManager::getSpriteSheet()`

### Shader Workflow
1. **Edit GLSL** shaders in `Resources/shaders/`
2. **Compile** using `compile_shaders.bat` (requires Vulkan SDK)
3. **Load** in engine via `Renderer::createShader()`

### ECS Workflow
1. **Create entities** via `EntityRegistry::createPlayer()`, `createSpriteEntity()`, etc.
2. **Add components** via registry methods or direct component assignment
3. **Run systems** in `onUpdate()` loop
4. **Render** via `Systems::renderSprites()`

---

## Performance Metrics

**Current Performance:**
- **FPS:** Consistent 59-61 FPS (VSYNC-limited)
- **Tile Rendering:** 3,600 tiles per frame with no performance impact
- **Resolution:** 1280x720 window
- **GPU:** NVIDIA GeForce RTX 3070 Ti, Vulkan 1.4.325
- **CPU:** 16 logical cores, 31.93GB RAM
- **OS:** Windows (SDL 3.5.0)

---

## Known Issues

1. **ResourceManager YAML Loading** - Complex YAML files (decor-grass.yaml, dirt-grass.yaml) crash on `loadTilesetFromAtlasYAML()`
   - **Workaround:** Use direct Texture loading for now
   - **Status:** Investigation needed for YAML parsing issue

2. **Plains Tileset Missing** - Original demo required non-existent `Resources/mystic_woods_2.2/sprites/tilesets/plains.yaml`
   - **Workaround:** Using decor-grass/dirt-grass sprite sheets as replacement

---

## Future Development Directions

### High Priority
1. **Audio System** - SDL3_audio integration for sound effects and music
2. **Post-Processing Completion** - Finish and test post-processing effects
3. **ResourceManager YAML Fix** - Fix complex YAML loading crashes
4. **Animation Enhancements** - More sophisticated animation controllers

### Medium Priority
5. **Physics Engine** - Integration with Box2D or similar
6. **Scene Management** - Scene graph or scene loading system
7. **Asset Pipeline** - Automated asset processing and optimization
8. **Performance Profiling** - Built-in profiling tools

### Low Priority
9. **Scripting Support** - Lua or similar scripting integration
10. **Networking** - Multiplayer support
11. **Editor Tools** - Level editor, sprite editor
12. **Platform Expansion** - Linux, macOS support

---

## Summary

**Runa2** is a **well-architected, modern C++20 game engine** with a solid foundation for 2D game development. The engine demonstrates:

- ✅ **Professional code organization** with clear module separation
- ✅ **Modern C++ best practices** (RAII, smart pointers, move semantics, C++20)
- ✅ **Extensible design patterns** (virtual hooks, DLL architecture, ECS)
- ✅ **Comprehensive documentation** (architecture, build, workflow)
- ✅ **Active logging** throughout the codebase
- ✅ **Declarative resource system** (YAML manifests)
- ✅ **Complete ECS implementation** with EnTT
- ✅ **Working rendering pipeline** with Vulkan2D GPU acceleration
- ✅ **Rich feature set** (input, camera, collision, tilemaps)

**Current State:**
- **Core Infrastructure:** ✅ Fully operational
- **Resource Management:** ✅ Complete (with YAML workaround)
- **Rendering:** ✅ Complete with Vulkan2D GPU pipeline
- **ECS System:** ✅ Fully implemented and functional
- **Game Systems:** ✅ Input, collision, camera all working
- **Audio:** ❌ Not yet implemented

The architecture is designed for easy extension, making it an excellent foundation for continued development. The main remaining work is adding audio support, fixing YAML resource loading, and potentially integrating a physics engine for more advanced game mechanics.

---

**Analysis Date:** 2025-01-03  
**Repository:** Runa2  
**Author:** soulwax@github  
**License:** AGPLv3
