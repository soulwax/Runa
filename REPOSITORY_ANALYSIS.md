# Runa2 Repository Analysis

**Version:** 1.0.6  
**Last Updated:** 2026-01-10  
**Language:** C++20  
**Build System:** CMake 3.20+ with Ninja

## Overview

Runa2 is a cross-platform 2D game engine built with modern C++20, featuring:
- **Vulkan2D** for hardware-accelerated rendering
- **SDL3** for windowing, input, and multimedia
- **EnTT** Entity Component System (ECS) for game object management
- **Scene-based architecture** for game state management
- **YAML-based configuration** for resources, keybindings, and scene serialization

The engine follows a **shared library architecture** where the engine (`Runa2Engine.dll`) is separate from the game executable (`Runa2.exe`), enabling modular development and potential reuse.

---

## Architecture

### Core Design Patterns

1. **Entity Component System (ECS)**
   - Uses **EnTT v3.13.2** for high-performance entity management
   - Component-based architecture (Position, Velocity, Sprite, Animation, etc.)
   - System-based processing (updatePlayerInput, updateMovement, renderSprites, etc.)
   - Tag components for entity classification (Player, Enemy, Active, etc.)

2. **Scene Management**
   - State machine pattern for game states
   - Lifecycle hooks: `onEnter()`, `onExit()`, `onPause()`, `onResume()`
   - Scene stack for layered rendering (e.g., pause overlay)
   - Scene serialization to/from YAML

3. **Resource Management**
   - YAML-based sprite sheet definitions
   - Centralized resource loading via `ResourceManager`
   - Sprite sheet system with animation support
   - Texture caching and management

4. **Rendering Pipeline**
   - **SpriteBatch** for efficient batched rendering
   - Global pixel scaling system (3x by default)
   - Camera system with zoom and world-to-screen conversion
   - Post-processing shader support

---

## Directory Structure

```
Runa2/
├── src/                          # Engine source code (Runa2Engine shared library)
│   ├── Core/                     # Core engine systems
│   │   ├── Application.h/cpp     # Main application loop and lifecycle
│   │   ├── Scene.h/cpp           # Base scene class
│   │   ├── SceneManager.h/cpp    # Scene stack management
│   │   ├── SceneSerializer.h/cpp # YAML scene save/load
│   │   ├── Input.h/cpp           # Input handling (keyboard, mouse)
│   │   ├── InputManager.h/cpp    # Input action system
│   │   ├── Keybindings.h/cpp     # YAML-based keybindings
│   │   ├── ResourceManager.h/cpp # Resource loading and caching
│   │   ├── Log.h/cpp             # Logging system (spdlog wrapper)
│   │   └── ...
│   │
│   ├── Graphics/                 # Rendering systems
│   │   ├── Renderer.h/cpp        # Vulkan2D renderer wrapper
│   │   ├── Window.h/cpp          # SDL3 window management
│   │   ├── SpriteBatch.h/cpp     # Batched sprite rendering
│   │   ├── SpriteSheet.h/cpp     # Sprite sheet and animation management
│   │   ├── Texture.h/cpp          # Texture loading and management
│   │   ├── Camera.h/cpp          # 2D camera with zoom
│   │   ├── Font.h/cpp            # TrueType font rendering (SDL3_ttf)
│   │   ├── PixelScale.h/cpp      # Global pixel scaling utilities
│   │   ├── TileMap.h/cpp          # Tile-based map rendering
│   │   ├── PostProcess.h/cpp     # Post-processing effects
│   │   └── ...
│   │
│   ├── ECS/                      # Entity Component System
│   │   ├── Components.h          # Core ECS components (Position, Velocity, Sprite, etc.)
│   │   ├── Systems.h/cpp         # ECS systems (movement, rendering, input, etc.)
│   │   ├── Registry.h/cpp         # Entity registry wrapper
│   │   ├── RPGComponents.h       # RPG-specific components (Health, Combat, Inventory, etc.)
│   │   └── RPGSystems.h/cpp      # RPG-specific systems
│   │
│   ├── Scenes/                   # Game scene implementations
│   │   ├── TestScene.h/cpp       # Main test scene (meadow, fences, player)
│   │   ├── GameScene.h/cpp       # Generic game scene
│   │   ├── MenuScene.h/cpp       # Menu scene
│   │   └── PauseScene.h/cpp      # Pause overlay scene
│   │
│   ├── Vulkan/                   # Vulkan2D renderer (integrated)
│   │   └── VK2D/                 # Vulkan2D source code
│   │
│   ├── RunaAPI.h                 # API export macros
│   ├── runapch.h/cpp             # Precompiled header
│   └── ...
│
├── Sandbox/                      # Game implementation (executable)
│   ├── main_game.cpp             # Main entry point (uses TestScene)
│   ├── main_rpg.cpp              # RPG demo
│   ├── main_input_demo.cpp       # Input system demo
│   ├── main_scene_demo.cpp       # Scene management demo
│   └── ...
│
├── Resources/                    # Game assets and configuration
│   ├── SpiteSheets/              # Sprite sheet definitions (YAML) and images (PNG)
│   │   ├── characters/           # Character sprites
│   │   │   └── player.yaml       # Player sprite definitions
│   │   ├── flowers.yaml          # Flower sprites
│   │   ├── fences.yaml           # Fence sprites
│   │   ├── grass.yaml            # Grass tile sprites
│   │   └── ...
│   ├── Fonts/                    # TrueType fonts
│   │   └── Renogare.ttf
│   ├── shaders/                  # GLSL shaders (vertex/fragment)
│   │   ├── sprite.vert/frag      # Sprite rendering
│   │   ├── post-processing/     # Various post-processing effects
│   │   └── ...
│   ├── keybindings.yaml          # Keybindings configuration
│   ├── game.yaml                 # Game settings
│   └── saves/                    # Scene save files
│
├── build/                        # Build output (gitignored)
│   └── debug/                    # Debug build artifacts
│
├── CMakeLists.txt                # Main CMake configuration
├── README.md                      # Project documentation
├── CHANGELOG.md                   # Version history
└── LICENSE.md                     # AGPLv3 license
```

---

## Key Technologies & Dependencies

### Core Dependencies (Auto-fetched via CMake FetchContent)

| Library | Version | Purpose |
|--------|---------|---------|
| **SDL3** | main | Windowing, events, input, audio |
| **SDL3_image** | main | Image loading (PNG, JPG) |
| **SDL3_ttf** | main | TrueType font rendering |
| **Vulkan2D** | integrated | 2D Vulkan renderer (in `src/Vulkan/`) |
| **EnTT** | v3.13.2 | Entity Component System |
| **yaml-cpp** | master | YAML parsing for configs |
| **spdlog** | v1.15.0 | Fast C++ logging |
| **nlohmann/json** | v3.11.3 | JSON parsing |
| **Dear ImGui** | v1.90.9 | Immediate mode GUI (Vulkan backend) |

### Build Tools

- **CMake** 3.20+ (build configuration)
- **Ninja** (build system)
- **GCC/G++** (C++20 compiler)
- **Vulkan SDK** (required for rendering)

---

## Core Systems

### 1. Application Loop

**File:** `src/Core/Application.h/cpp`

- Main game loop with fixed timestep
- Window and renderer initialization
- Scene manager integration
- FPS tracking

**Lifecycle:**
```cpp
onInit() → mainLoop() → onUpdate(dt) → onRender() → onShutdown()
```

### 2. Entity Component System

**Files:** `src/ECS/Components.h`, `src/ECS/Systems.h/cpp`, `src/ECS/Registry.h/cpp`

**Core Components:**
- `Position` - World coordinates (x, y)
- `Velocity` - Movement velocity (x, y)
- `Size` - Entity dimensions (width, height)
- `Sprite` - Sprite sheet reference, sprite name, tint, flip flags
- `Animation` - Frame index, animation time, frame rate, loop flag
- `PlayerInput` - Movement speed
- `CameraTarget` - Camera follow smoothing
- `AABB` - Axis-aligned bounding box for collision
- `Active` - Tag component for active entities
- `Player` - Tag component for player entities

**Core Systems:**
- `updatePlayerInput()` - Processes keyboard input, sets velocity
- `updateMovement()` - Applies velocity to position
- `updateAnimation()` - Advances animation frames based on time
- `renderSprites()` - Renders all entities with sprites
- `updateCameraFollow()` - Smooth camera following

### 3. Rendering System

**Files:** `src/Graphics/SpriteBatch.h/cpp`, `src/Graphics/Renderer.h/cpp`

**Features:**
- Batched sprite rendering via Vulkan2D
- Global pixel scaling (3x by default - each pixel becomes 3x3)
- Sprite flipping (horizontal/vertical mirroring)
- Texture tinting (RGBA)
- Camera-based culling for performance

**Pixel Scaling:**
- Centralized in `PixelScale` utility class
- Applied automatically in `SpriteBatch::draw()`
- UI elements can be unscaled for consistent size

**Camera System:**
- World-to-screen coordinate conversion
- Zoom support (default 3.0f)
- Smooth following with configurable smoothing
- Viewport bounds calculation for culling

### 4. Scene Management

**Files:** `src/Core/Scene.h/cpp`, `src/Core/SceneManager.h/cpp`

**Scene Lifecycle:**
```cpp
onEnter()    // Called when scene is pushed
onUpdate(dt) // Called every frame
onRender()   // Called every frame
onPause()    // Called when scene is paused (overlay pushed)
onResume()   // Called when scene is resumed
onExit()     // Called when scene is popped
```

**Scene Types:**
- `TestScene` - Main gameplay scene (meadow, fences, player)
- `GameScene` - Generic game scene
- `MenuScene` - Menu/UI scene
- `PauseScene` - Pause overlay (transparent)

### 5. Input System

**Files:** `src/Core/Input.h/cpp`, `src/Core/Keybindings.h/cpp`

**Features:**
- Keyboard and mouse input via SDL3
- YAML-based keybindings configuration
- Action-based input system (multiple keys per action)
- Gamepad support (via `GamepadManager`)

**Keybindings File:** `Resources/keybindings.yaml`
- Human-readable YAML format
- Supports multiple keys per action (e.g., WASD + arrow keys)
- Easy to customize without recompiling

### 6. Resource Management

**Files:** `src/Core/ResourceManager.h/cpp`, `src/Graphics/SpriteSheet.h/cpp`

**Sprite Sheet System:**
- YAML definitions for sprite sheets
- Individual sprite definitions with coordinates
- Animation support (frame sequences, frame rates, looping)
- Automatic texture loading from PNG files

**Example YAML Structure:**
```yaml
texture: "path/to/texture.png"
sprites:
  - name: "sprite_name"
    atlas_x: 0
    atlas_y: 0
    width: 16
    height: 16
animations:
  - name: "walk_animation"
    start_x: 0
    start_y: 0
    width: 16
    height: 16
    frame_count: 6
    frame_rate: 10.0
    loop: true
```

### 7. Scene Serialization

**Files:** `src/Core/SceneSerializer.h/cpp`

**Features:**
- Save entire scene state to YAML
- Load scene state from YAML
- Serializes all ECS entities and components
- Preserves sprite sheet references
- Human-readable format (editable in text editors)
- Hotkeys: F5 (save), F6 (load)

**Supported Components:**
- Position, Velocity, Size
- Sprite, Animation
- PlayerInput, AABB
- Player tag, CameraTarget

---

## Game Implementation (Sandbox)

### Main Entry Point

**File:** `Sandbox/main_game.cpp`

Creates a `Game` class that extends `Application` and pushes `TestScene` as the initial scene.

### TestScene

**File:** `src/Scenes/TestScene.h/cpp`

**Features:**
- Meadow generation with grass tiles
- Fence placement around meadow (square perimeter)
- Decorative grass and flowers scattered across field
- Player entity with walking animations
- Camera following player
- FPS counter display
- Scene save/load (F5/F6)

**Sprite Sheets Used:**
- Base grass tiles
- Decorative grass blades
- Flowers (7 types)
- Fences (wooden, with connection logic)
- Player character (idle/walk animations, 4 directions)

---

## Configuration Files

### `Resources/keybindings.yaml`
- Defines all game keybindings
- Supports multiple keys per action
- Format: `action_name: [SDL_KEYCODE_*]`

### `Resources/game.yaml`
- Game settings (camera zoom, etc.)

### `Resources/SpiteSheets/*.yaml`
- Sprite sheet definitions
- Sprite coordinates and dimensions
- Animation sequences

---

## Build System

### CMake Configuration

**Architecture:**
- **Runa2Engine** - Shared library (`.dll` on Windows, `.so` on Linux)
- **Runa2** - Executable that links to the engine

**Build Types:**
- **Debug:** `-g -O0` (full debug symbols, no optimization)
- **Release:** `-O3` (maximum optimization)

**Output:**
- All binaries in `build/debug/` or `build/release/`
- Shared libraries automatically copied to executable directory

### Compilation

```bash
# Configure
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug

# Build
cmake --build build/debug

# Run
./build/debug/Runa2
```

---

## Notable Patterns & Conventions

### 1. Namespace Organization
- `Runa::` - Main engine namespace
- `Runa::ECS::` - ECS components and systems
- `Runa::ECS::Systems::` - ECS system functions

### 2. Component Design
- Components are plain structs (POD types)
- Tag components are empty structs (`struct Player {};`)
- Components use `RUNA_API` macro for DLL export

### 3. System Design
- Systems are free functions in `ECS::Systems` namespace
- Systems operate on entity views (component filters)
- Systems are stateless (all state in components)

### 4. Resource Loading
- YAML-based definitions for flexibility
- Sprite sheets loaded from PNG + YAML pairs
- Resources cached in `ResourceManager`

### 5. Coordinate Systems
- **World Space:** Logical units (e.g., 16x16 tiles)
- **Screen Space:** Physical pixels (after zoom)
- **Pixel Scale:** Global 3x multiplier (logical → screen)
- **Camera Zoom:** Additional scaling factor (default 3.0f)

**Important:** Offset calculations must account for both pixel scale and zoom when converting between coordinate systems.

### 6. Rendering Pipeline
1. `Renderer::beginFrame()` - Start frame
2. `SpriteBatch::begin()` - Start sprite batch
3. `SpriteBatch::draw()` - Queue sprites (multiple calls)
4. `SpriteBatch::end()` - Flush batch to GPU
5. `Renderer::endFrame()` - Present frame

---

## Recent Changes (v1.0.6)

### Fixed
- Entity rendering offset calculation (double-scaling bug)
- Version consistency between CMakeLists.txt and CHANGELOG.md
- Sprite rendering vs. fallback rendering offset consistency

### Added (v1.0.5)
- Keybindings metadata system (YAML-based)
- Sprite flipping (horizontal/vertical)
- Scene serialization (save/load to YAML)

---

## Development Workflow

1. **Engine Code:** Modify files in `src/`
2. **Game Code:** Modify files in `Sandbox/` or `src/Scenes/`
3. **Assets:** Add/modify files in `Resources/`
4. **Build:** Use CMake + Ninja (or VSCode tasks)
5. **Debug:** Use VSCode debugger (F5) or GDB

---

## Testing & Demos

The `Sandbox/` directory contains multiple demo entry points:
- `main_game.cpp` - Main game (TestScene)
- `main_rpg.cpp` - RPG demo with combat system
- `main_input_demo.cpp` - Input system demonstration
- `main_scene_demo.cpp` - Scene management demonstration

---

## Future Considerations

Based on the codebase structure, potential areas for expansion:
- Audio system (SDL3_mixer integration)
- Physics system (collision detection/resolution)
- Networking (multiplayer support)
- Scripting system (Lua integration)
- Asset pipeline tools
- Editor application

---

## License

**AGPLv3** - See `LICENSE.md` for details.

---

## Author

soulwax@github

---

*This analysis was generated on 2026-01-10. For the most up-to-date information, refer to the source code and CHANGELOG.md.*
