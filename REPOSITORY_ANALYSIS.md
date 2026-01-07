# Runa2 Game Engine - Repository Analysis

## Executive Summary

**Runa2** is a cross-platform C++20 2D game engine built with SDL3 and Vulkan2D. It features a modern Entity Component System (ECS) architecture, comprehensive RPG systems, scene management, and hardware-accelerated rendering. The engine is currently demonstrated through "The Slime Hunter RPG" - a top-down action RPG with combat, quests, inventory, and AI systems.

**Version**: 1.0.0  
**License**: AGPLv3  
**Author**: soulwax@github

---

## Core Technologies & Dependencies

### Primary Technologies
- **C++20** - Modern C++ standard (required, no extensions)
- **CMake 3.20+** - Build system with Ninja generator
- **GCC/G++** (or Clang on macOS) - Compiler toolchain
- **Vulkan** - Graphics API (via Vulkan2D abstraction)

### Core Dependencies (Auto-fetched via CMake FetchContent)
- **SDL3** (main branch) - Windowing, events, input handling
- **SDL3_image** (main branch) - PNG/JPG image loading
- **SDL3_ttf** (main branch) - TrueType font rendering (vendored FreeType)
- **EnTT v3.13.2** - Header-only Entity Component System library
- **yaml-cpp** (master) - YAML parsing for resource manifests
- **spdlog v1.15.0** - Fast C++ logging library
- **nlohmann/json v3.11.3** - JSON library (available but not actively used)

### Integrated Libraries
- **Vulkan2D** - 2D Vulkan renderer (Git submodule in `src/Vulkan2D/`)
  - 23 C source files (~350KB)
  - Includes VulkanMemoryAllocator for GPU memory management
  - Provides VK2D API abstraction over Vulkan

---

## Project Architecture

### Build Architecture
The project uses a **shared library architecture**:

1. **Runa2Engine** (SHARED) - Engine core library
   - Contains all engine systems (Core, Graphics, ECS, Scenes)
   - Exports symbols via `RUNA_API` macro
   - Output: `libRuna2Engine.dll` (Windows), `.so` (Linux), `.dylib` (macOS)

2. **Runa2** (EXECUTABLE) - Game application
   - Links against Runa2Engine shared library
   - Currently uses `main_rpg.cpp` as entry point
   - Alternative entry points available: `main.cpp`, `main_scene_demo.cpp`, `main_input_demo.cpp`

### Directory Structure

```
Runa2/
├── src/                          # Source code
│   ├── Core/                     # Core engine systems
│   │   ├── Application.h/cpp    # Base application class with lifecycle
│   │   ├── Scene.h/cpp           # Scene base class
│   │   ├── SceneManager.h/cpp   # Scene stack management
│   │   ├── ResourceManager.h/cpp # YAML-based resource loading
│   │   ├── Log.h/cpp            # spdlog wrapper
│   │   ├── Input.h/cpp          # Frame-based keyboard/mouse input
│   │   ├── InputManager.h/cpp   # Action-based input system with bindings
│   │   ├── GamepadManager.h/cpp  # Gamepad support
│   │   ├── Entity.h/cpp         # Legacy entity class (superseded by ECS)
│   │   ├── PlayerController.h/cpp # Legacy player controller
│   │   └── Collision.h/cpp      # AABB collision detection
│   │
│   ├── Graphics/                 # Rendering systems
│   │   ├── Window.h/cpp         # SDL3 window management
│   │   ├── Renderer.h/cpp       # Vulkan2D renderer wrapper
│   │   ├── SpriteBatch.h/cpp    # Batched sprite rendering
│   │   ├── Texture.h/cpp        # Texture wrapper
│   │   ├── SpriteSheet.h/cpp    # Sprite atlas management
│   │   ├── TileMap.h/cpp        # Grid-based tile rendering
│   │   ├── Camera.h/cpp         # 2D camera with entity following
│   │   ├── Font.h/cpp           # TrueType font rendering
│   │   ├── Shader.h/cpp         # Shader management
│   │   ├── PostProcess.h/cpp    # Post-processing (stub, deprecated)
│   │   └── SimpleShapes.h/cpp   # Basic shape rendering
│   │
│   ├── ECS/                      # Entity Component System
│   │   ├── Components.h         # Basic components (Position, Velocity, Sprite, etc.)
│   │   ├── RPGComponents.h      # RPG components (Health, Combat, Inventory, Quest, etc.)
│   │   ├── Registry.h/cpp       # EntityRegistry wrapper
│   │   ├── Systems.h/cpp        # Basic ECS systems (movement, rendering, collision)
│   │   └── RPGSystems.h/cpp     # RPG systems (combat, AI, quests, items)
│   │
│   ├── Scenes/                   # Built-in scene implementations
│   │   ├── MenuScene.h/cpp       # Main menu
│   │   ├── GameScene.h/cpp      # Gameplay scene
│   │   └── PauseScene.h/cpp     # Pause overlay
│   │
│   ├── Vulkan2D/                 # Vulkan2D renderer (Git submodule)
│   │   ├── VK2D/include/        # Public API headers
│   │   └── VK2D/src/             # Implementation (23 C files)
│   │
│   ├── main_rpg.cpp              # ACTIVE: The Slime Hunter RPG demo
│   ├── main.cpp                  # Basic ECS demo
│   ├── main_scene_demo.cpp       # Scene system demo
│   ├── main_input_demo.cpp       # Input system demo
│   ├── runapch.h/cpp             # Precompiled header
│   └── RunaAPI.h                 # Shared library export macros
│
├── Resources/                    # Game assets
│   ├── Fonts/                    # TrueType fonts (Renogare.otf/ttf)
│   ├── manifests/                # Resource manifests (YAML)
│   ├── mystic_woods_2.2/         # Tileset assets
│   ├── SpiteSheets/              # Sprite sheets
│   └── shaders/                  # GLSL shaders (30+ effects)
│       ├── *.vert/*.frag         # Source files
│       └── compiled/*.spv        # Compiled SPIR-V binaries
│
├── build/                        # Build output (gitignored)
│   ├── debug/                    # Debug builds (-g -O0)
│   └── release/                  # Release builds (-O3)
│
├── vendor/                       # Third-party libraries
│   ├── JSON/                     # nlohmann/json
│   └── Vulkan2D/                 # Vulkan2D renderer
│
├── CMakeLists.txt                # Build configuration
├── README.md                     # User documentation
├── CLAUDE.md                     # AI assistant guide
└── CHANGELOG.md                  # Version history
```

---

## Key Features & Systems

### 1. Entity Component System (ECS)

**Framework**: EnTT v3.13.2 (header-only, modern C++)

#### Basic Components (`Components.h`)
- **Transform & Physics**: `Position`, `Velocity`, `Size`, `Transform`
- **Rendering**: `Sprite`, `Animation`
- **Collision**: `AABB`, `CollisionLayer`
- **Input**: `PlayerInput`, `CameraTarget`
- **Tags**: `Player`, `Enemy`, `Projectile`, `Pickup`, `Static`, `Active`

#### RPG Components (`RPGComponents.h`)
- **Stats**: `Health`, `Combat`, `Experience`
- **AI**: `AIController` (Idle, Patrol, Chase, Attack, Flee, Dead states)
- **Items**: `Item`, `DroppedItem`, `Inventory`
- **Quests**: `Quest`, `QuestGiver`
- **UI**: `DamageNumber` (floating damage text)

#### ECS Systems
- **Basic Systems** (`Systems.h/cpp`):
  - `updatePlayerInput()` - Reads input, sets velocities
  - `updateMovement()` - Applies velocity to position
  - `updateAnimation()` - Updates animation frames
  - `updateTileCollisions()` - Resolves tilemap collisions
  - `renderSprites()` - Renders all sprites with camera transform

- **RPG Systems** (`RPGSystems.h/cpp`):
  - `updateAI()` - Enemy AI state machine
  - `updateCombat()` - Attack detection, damage calculation
  - `updateItemCollection()` - Item pickup logic
  - `updateQuests()` - Quest progress tracking
  - `updateDamageNumbers()` - Floating damage text animation
  - `renderPlayerUI()` - Health bar, XP bar, inventory display
  - `renderDamageNumbers()` - Renders floating damage text

#### EntityRegistry
Wrapper around `entt::registry` with helper functions:
- `createEntity(x, y)` - Creates basic entity
- `createPlayer(x, y, spriteSheet, spriteName, speed)` - Creates player entity
- `addSprite()`, `addAnimation()`, `addCollision()` - Component helpers

### 2. Graphics & Rendering

**Backend**: Vulkan2D (VK2D) - Hardware-accelerated 2D rendering via Vulkan

#### Rendering Pipeline
1. **Renderer** - Wraps VK2DRenderer, manages frame lifecycle
   - `beginFrame()` / `endFrame()` - Frame boundaries (called by Application)
   - `clear(r, g, b, a)` - Clear screen color
   - VSYNC enabled, NEAREST filtering, MSAA disabled

2. **SpriteBatch** - Automatic sprite batching
   - Batches multiple draws into single draw calls
   - Supports textures, sprite sheets, tinting, rotation
   - Performance: 3,600 tiles @ 60 FPS (tested on RTX 3070 Ti)

3. **Texture** - Texture loading and management
   - Loads via SDL3_image (PNG, JPG)
   - Wraps VK2DTexture

4. **Camera** - 2D camera system
   - Entity following with smooth interpolation
   - World-to-screen / screen-to-world transforms
   - Visible bounds calculation for frustum culling
   - Methods: `follow()`, `followEntity()`, `worldToScreen()`, `getWorldBounds()`

5. **TileMap** - Grid-based tile rendering
   - Per-tile solidity flags
   - Efficient tile lookup and rendering

6. **Font** - TrueType font rendering
   - Renders text to textures
   - Supports custom colors

#### Shader System
**30+ GLSL shaders** available in `Resources/shaders/`:

- **Visual Effects**: psychedelic, CRT, pixelate, bloom, kaleidoscope, water
- **Color Grading**: day_night, grayscale, sepia, vignette, blur
- **Gameplay Effects**: damage_flash, dissolve, outline, ghost, fade
- **Status Effects**: freeze, poison, lightning, heat_distortion
- **Magic Effects**: glow, shield, portal
- **Utility**: palette_swap

Shaders compile to SPIR-V via `glslc` (Vulkan SDK). Compilation scripts: `compile_shaders.bat` / `compile_shaders.sh`

**Note**: Post-processing system is currently a stub (`PostProcess.h/cpp`). Shaders exist but integration with Vulkan2D pipeline is not implemented.

### 3. Input System

#### Dual Input Systems

1. **Input** (`Input.h/cpp`) - Frame-based direct input
   - `isKeyDown()`, `isKeyPressed()`, `isKeyReleased()`
   - `isMouseButtonDown()`, `isMouseButtonPressed()`, `isMouseButtonReleased()`
   - `getMousePosition(x, y)`

2. **InputManager** (`InputManager.h/cpp`) - Action-based input system
   - **Input Actions**: Named actions (e.g., "Move", "Attack", "Interact")
   - **Input Bindings**: JSON-based key bindings (`Resources/input_bindings.json`)
   - **Contexts**: Multiple input contexts (e.g., "Gameplay", "Menu")
   - **2D Axes**: WASD/Arrow keys mapped to 2D movement axes
   - **Gamepad Support**: Via `GamepadManager`

**Current RPG Demo** uses InputManager with actions:
- "Move" (WASD 2D axis)
- "Attack" (SPACE)
- "Interact" (E)
- "ToggleInventory" (I)

### 4. Scene Management

**Stack-based scene system** for game state management:

- **Scene Base Class** - Lifecycle hooks: `onEnter()`, `onExit()`, `onPause()`, `onResume()`
- **SceneManager** - Manages scene stack
  - `pushScene()` - Add overlay (e.g., pause menu)
  - `popScene()` - Remove top scene
  - `changeScene()` - Replace current scene
  - `clearScenes()` - Clear all scenes
- **Transparent Scenes** - Render underlying scenes (for overlays)

**Built-in Scenes**:
- `MenuScene` - Main menu with text rendering
- `GameScene` - Full gameplay with ECS
- `PauseScene` - Overlay pause menu

### 5. Resource Management

**ResourceManager** (`ResourceManager.h/cpp`) - YAML-based asset loading:

- **Tileset Loading**: `loadTilesetFromAtlasYAML(yamlPath, imagePath, name)`
- **Sprite Lookup**: `getSprite(tilesetName, spriteName)`
- **Manifest System**: `Resources/manifests/resource_manifest.yaml`

**YAML Format**:
```yaml
meta:
  tile_size: 16
  frames: 1
tiles:
  - id: sprite_name
    atlas_x: 16
    atlas_y: 16
    tile_size: 16
```

**Known Issue**: Complex YAML files (20+ tiles) cause silent crashes. Workaround: Use direct `Texture` loading.

### 6. Collision System

**Collision** (`Collision.h/cpp`) - AABB collision detection:

- `checkAABB()` - Test two AABBs
- `resolveAABB()` - Push one box out of another
- `checkTileCollisions()` - Entity vs tilemap collision resolution
- **CollisionManager** - Bulk collision checks

### 7. Logging System

**Log** (`Log.h/cpp`) - spdlog wrapper with macros:

- `LOG_TRACE()`, `LOG_DEBUG()`, `LOG_INFO()`, `LOG_WARN()`, `LOG_ERROR()`, `LOG_CRITICAL()`
- File logging to `logs/runa2.log`
- Console output with colors

---

## Current Demo: The Slime Hunter RPG

**Entry Point**: `src/main_rpg.cpp`

### Gameplay Features
- **Player Character**: Blue square, WASD movement, SPACE attack
- **Enemies**: Green slimes with AI (patrol, chase, attack)
- **Items**: Red potions (heal 30 HP), Yellow coins (5 gold)
- **Quest System**: NPC quest giver, "Defeat 5 slimes" quest
- **Inventory**: Press I to toggle, displays collected items
- **Combat**: Melee attacks, damage numbers, health bars
- **Experience**: Leveling system with XP rewards
- **Camera**: Follows player with smooth interpolation
- **World**: 50x50 tile procedurally generated map with obstacles

### Systems Integration
- ECS for all entities (player, enemies, items, NPCs)
- InputManager for action-based controls
- RPGSystems for combat, AI, quests, items
- Camera system for world-to-screen transforms
- Font rendering for UI text

---

## Build System

### Configuration
- **Generator**: Ninja
- **Compiler**: GCC/G++ (or Clang on macOS)
- **C++ Standard**: C++20 (required, no extensions)
- **Debug Flags**: `-g -O0` (full symbols, no optimization)
- **Release Flags**: `-O3` (maximum optimization)

### Build Commands
```bash
# Configure Debug
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/debug

# Build
cmake --build build/debug

# Run
./build/debug/Runa2.exe  # Windows
./build/debug/Runa2      # Linux/macOS
```

### First Build
- Takes **5-15 minutes** (downloads and compiles SDL3, Vulkan2D, dependencies)
- Requires Git in PATH for FetchContent
- Requires Vulkan runtime (usually included with GPU drivers)

### VSCode Integration
- **F5** - Build and debug
- **Ctrl+Shift+B** - Build only
- Tasks configured for Debug/Release builds
- Launch configs for debugging with GDB
- IntelliSense via `compile_commands.json`

---

## Notable Patterns & Design Decisions

### 1. Shared Library Architecture
- Engine code in shared library (`Runa2Engine`)
- Game code in executable (`Runa2`)
- Enables multiple games using same engine
- DLL/shared library management handled by CMake

### 2. Dual Entity Systems
- **Legacy**: `Entity` base class with inheritance (`Entity.h`, `PlayerController.h`)
- **Modern**: ECS with EnTT (`ECS/` folder)
- ECS preferred for new code (better performance, flexibility)
- Legacy system maintained for backward compatibility

### 3. Precompiled Headers
- `runapch.h/cpp` - Must be first include in all `.cpp` files
- Includes common SDL3, STL, spdlog headers
- Critical: Include order matters (spdlog before sinks)

### 4. Namespace Organization
- `Runa::` - Core engine namespace
- `Runa::ECS::` - ECS components and systems
- `Runa::Graphics::` - Graphics systems (implicit via `Runa::`)

### 5. Export Macros
- `RUNA_API` - Shared library export/import (`RunaAPI.h`)
- `RUNA2_ENGINE_EXPORTS` - Defined when building engine library
- Ensures proper symbol visibility across DLL boundaries

---

## Known Issues & Limitations

### 1. ResourceManager YAML Loading Crashes
- **Symptom**: Silent crash with complex YAML files (20+ tiles)
- **Affected**: `decor-grass.yaml`, `dirt-grass.yaml`
- **Workaround**: Use direct `Texture` loading
- **Status**: Under investigation

### 2. Post-Processing Not Integrated
- **Shaders**: 30+ GLSL shaders exist in `Resources/shaders/`
- **Integration**: `PostProcess` class is a stub
- **Reason**: Vulkan2D uses internal pipeline, custom post-processing requires custom implementation
- **Status**: Shaders available but not usable

### 3. EnTT API Compatibility
- Use `view.size_hint()` instead of `view.empty()` (EnTT v3.13.2)
- Component headers must be included explicitly outside `ECS/` folder

### 4. Texture Path Requirements
- Working directory must be project root
- Use relative paths WITHOUT leading slash: `"Resources/path.png"` ✓
- Wrong: `"/Resources/path.png"` ✗

---

## Performance Characteristics

### Tested Configuration
- **GPU**: NVIDIA GeForce RTX 3070 Ti
- **Vulkan**: 1.4.325
- **Resolution**: 1280x720
- **FPS**: Consistent 59-61 FPS (VSYNC-limited)

### Rendering Performance
- **3,600 tiles** rendered per frame with no performance impact
- Automatic sprite batching via Vulkan2D
- Camera frustum culling for visible tiles only

### Build Performance
- **First build**: 5-15 minutes (dependency compilation)
- **Incremental builds**: Fast (only changed files recompile)
- **CMake configure**: ~10-30 seconds

---

## Development Workflow

### Typical Development Cycle
1. **Edit code** in `src/` directory
2. **Build** via F5 (VSCode) or `cmake --build build/debug`
3. **Debug** with breakpoints in VSCode
4. **Test** by running executable

### Code Organization
- **Engine code** in `src/Core/`, `src/Graphics/`, `src/ECS/`
- **Game code** in `src/main_*.cpp` files
- **Assets** in `Resources/`
- **Build output** in `build/` (gitignored)

### IntelliSense Setup
- `compile_commands.json` auto-generated by CMake
- Clangd or C/C++ extension uses compile commands
- **Critical**: Restart clangd after changing build configs

---

## Future Development Areas

### Planned Features (from CHANGELOG)
- Fix ResourceManager YAML loading crash
- Implement camera following system (partially done)
- Add collision detection with walls/obstacles (partially done)
- Add enemy entities (done)
- Add sprite animation system (partially done)
- Implement tile-based collision maps (done)
- Add sound system
- Add particle effects

### Potential Improvements
- Post-processing pipeline integration
- Audio system (SDL3_mixer or similar)
- Physics system (box2d integration?)
- Save/load system
- Networking/multiplayer
- Scripting system (Lua?)

---

## Summary

Runa2 is a **mature, feature-rich 2D game engine** with:
- ✅ Modern ECS architecture (EnTT)
- ✅ Hardware-accelerated rendering (Vulkan2D)
- ✅ Comprehensive RPG systems
- ✅ Scene management
- ✅ Action-based input system
- ✅ Resource management (YAML manifests)
- ✅ Extensive shader library (30+ effects)
- ✅ Cross-platform support (Windows, Linux, macOS)
- ✅ Well-documented codebase

The engine is actively developed and demonstrated through "The Slime Hunter RPG" - a fully playable top-down action RPG showcasing all major systems.

**Current Status**: Production-ready for 2D game development, with some known limitations (YAML loading, post-processing integration) that have workarounds.
