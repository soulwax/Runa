# Runa2 Repository Analysis

**Last Updated:** January 2025
**Version:** 1.0.0
**Language:** C++20
**Platform:** Cross-platform (Windows, Linux, macOS)

---

## Executive Summary

Runa2 is a cross-platform 2D game engine built with modern C++20, featuring:
- **Vulkan2D** for hardware-accelerated 2D rendering
- **SDL3** for windowing, events, and input
- **EnTT** Entity Component System (ECS) architecture
- **Dear ImGui** for immediate-mode GUI (recently integrated)
- Scene-based architecture with resource management
- RPG gameplay systems (combat, inventory, quests, AI)

The engine is designed for rapid game development with a clean, modular architecture and automatic dependency management via CMake FetchContent.

---

## Architecture Overview

### Core Architecture Pattern
- **Shared Library Engine** (`Runa2Engine` DLL) in `src/` directory
- **Game Implementation** (`Runa2` executable) in `Sandbox/` directory
- **Application Base Class** pattern for game initialization
- **Scene Management** system for organizing game states
- **ECS (Entity Component System)** using EnTT for game logic
- **Resource Management** with YAML manifests

### Code Organization
- **Engine Code** (`src/`): All code in `Runa::` namespace goes into the DLL
- **Game Code** (`Sandbox/`): Game implementations that use the engine API
- **Clear Separation**: Engine is reusable, game code is application-specific

### Key Design Patterns
1. **Component-Based Architecture**: ECS with EnTT
2. **Scene System**: MenuScene, GameScene, PauseScene
3. **Resource Loading**: YAML-based sprite sheet definitions
4. **Input Binding System**: Context-aware input with action mapping
5. **Renderer Abstraction**: Vulkan2D backend with SpriteBatch API

---

## Technology Stack

### Core Dependencies (via CMake FetchContent)
- **SDL3** (main branch) - Windowing, events, input
- **SDL3_image** (main branch) - Image loading (PNG, JPG, etc.)
- **SDL3_ttf** (main branch) - TrueType font rendering
- **Vulkan2D** (integrated in `src/Vulkan/`) - 2D Vulkan renderer
- **EnTT v3.13.2** - Entity Component System
- **spdlog v1.15.0** - Fast C++ logging
- **yaml-cpp** (master) - YAML parsing
- **nlohmann/json v3.11.3** - JSON library
- **Dear ImGui v1.90.9** - Immediate-mode GUI (recently integrated)

### Build System
- **CMake 3.20+** with Ninja generator
- **GCC/G++** (C++20 compatible) or Clang
- **Vulkan SDK** required for Vulkan2D

---

## Project Structure

```
Runa2/
├── src/                         # Engine code (Runa2Engine DLL)
│   ├── Core/                    # Core engine systems
│   │   ├── Application.{h,cpp} # Main application base class
│   │   ├── Input.{h,cpp}        # Input abstraction
│   │   ├── InputManager.{h,cpp} # Context-aware input binding
│   │   ├── InputBinding.h       # Input action definitions
│   │   ├── Scene.{h,cpp}        # Scene base class
│   │   ├── SceneManager.{h,cpp} # Scene state management
│   │   ├── ResourceManager.{h,cpp} # Resource loading (YAML, textures)
│   │   ├── Log.{h,cpp}          # Logging system (spdlog wrapper)
│   │   └── Collision.{h,cpp}    # Collision detection utilities
│   │
│   ├── Graphics/                # Rendering systems
│   │   ├── Window.{h,cpp}       # SDL3 window wrapper
│   │   ├── Renderer.{h,cpp}     # Vulkan2D renderer wrapper
│   │   ├── SpriteBatch.{h,cpp}  # Batched sprite rendering
│   │   ├── Texture.{h,cpp}      # Texture loading/management
│   │   ├── SpriteSheet.{h,cpp}  # Sprite sheet parsing (YAML)
│   │   ├── Camera.{h,cpp}        # 2D camera with world-to-screen transform
│   │   ├── Font.{h,cpp}          # SDL3_ttf font rendering
│   │   ├── TileMap.{h,cpp}      # Tile-based map system
│   │   ├── ImGui.{h,cpp}        # Dear ImGui integration (NEW)
│   │   ├── PostProcess.{h,cpp}  # Post-processing effects
│   │   └── SimpleShapes.{h,cpp} # Utility for colored rectangles
│   │
│   ├── ECS/                     # Entity Component System
│   │   ├── Components.h         # Core ECS components (Position, Velocity, etc.)
│   │   ├── Systems.{h,cpp}     # Core ECS systems (movement, rendering, etc.)
│   │   ├── Registry.{h,cpp}     # EntityRegistry wrapper for EnTT
│   │   ├── RPGComponents.h      # RPG-specific components (Health, Combat, etc.)
│   │   └── RPGSystems.{h,cpp}   # RPG gameplay systems (combat, AI, quests)
│   │
│   ├── Scenes/                  # Engine scene base classes
│   │   ├── MenuScene.{h,cpp}    # Main menu scene
│   │   ├── GameScene.{h,cpp}    # Main gameplay scene
│   │   └── PauseScene.{h,cpp}   # Pause menu scene
│   │
│   ├── Vulkan/                  # Vulkan2D renderer (integrated)
│   │   └── VK2D/                # Vulkan2D source code
│   │
│   ├── runapch.h                # Precompiled header
│   └── RunaAPI.h                # API export macros
│
├── Sandbox/                     # Game implementation (uses engine)
│   ├── main_rpg.cpp             # RPG demo entry point (PRIMARY)
│   ├── main.cpp                  # Basic ECS demo entry point
│   ├── main_input_demo.cpp      # Input system demo
│   ├── main_scene_demo.cpp      # Scene system demo
│   └── README.md                 # Sandbox documentation
│
├── Resources/                   # Game assets
│   ├── Fonts/                   # TTF font files
│   ├── shaders/                 # GLSL shader files
│   ├── SpiteSheets/             # Sprite sheets and YAML definitions
│   └── manifests/               # Resource manifest files
│
├── CMakeLists.txt               # Main build configuration
└── README.md                    # Project documentation
```

---

## Key Features

### 1. Graphics Pipeline

**Rendering Backend:** Vulkan2D
- Hardware-accelerated 2D rendering via Vulkan
- Automatic sprite batching for optimal performance
- NEAREST filtering for pixel-perfect rendering
- VSYNC enabled, MSAA disabled
- Triple buffering (3 swap chain images)

**Rendering Flow:**
```cpp
renderer.beginFrame();        // Start frame, clear screen
spriteBatch.begin();          // Begin sprite batching
// ... draw calls ...
spriteBatch.end();            // Flush batch to GPU
renderer.endFrame();          // Present frame
```

**Sprite Rendering:**
- Entities with sprite sheets: Rendered via `SpriteBatch::draw()` with texture and frame data
- Entities without sprite sheets: Rendered as colored rectangles using white pixel texture
- Camera system: World-to-screen coordinate transformation
- Tile rendering: Colored rectangles based on tile type

### 2. Entity Component System (ECS)

**Core Components** (`Components.h`):
- `Position` - World coordinates (x, y)
- `Velocity` - Movement velocity (x, y)
- `Size` - Entity dimensions (width, height)
- `Sprite` - Rendering data (spriteSheet, spriteName, tint colors)
- `Animation` - Animation state (currentFrame, animationTime, frameRate)
- `AABB` - Axis-aligned bounding box for collision
- `Active` - Entity active flag
- `Player` - Player tag component
- `PlayerInput` - Player input configuration
- `CameraTarget` - Camera follow target

**RPG Components** (`RPGComponents.h`):
- `Health` - HP system (current, max, isDead)
- `Combat` - Combat stats (damage, attackRange, cooldown)
- `Experience` - XP and leveling system
- `Inventory` - Item storage and gold
- `Item` - Item data (type, name, description, value)
- `Enemy` - Enemy tag component
- `AIController` - AI behavior (Idle, Patrol, Chase, Attack, Flee, Dead)
- `DroppedItem` - World item entity
- `QuestGiver` - NPC quest system
- `DamageNumber` - Floating damage text

**Core Systems** (`Systems.cpp`):
- `updatePlayerInput()` - Process keyboard input, set velocities
- `updateMovement()` - Apply velocity to position
- `updateAnimation()` - Update sprite animation frames
- `updateTileCollisions()` - Tile-based collision detection
- `updateEntityCollisions()` - Entity-to-entity collision
- `renderSprites()` - Render all sprites with camera transform
- `updateCameraFollow()` - Smooth camera following

**RPG Systems** (`RPGSystems.cpp`):
- `updateCombat()` - Player/enemy combat logic
- `updateAI()` - Enemy AI behavior (chase, attack, patrol)
- `updateItemCollection()` - Item pickup system
- `updateQuests()` - Quest completion tracking
- `updateDamageNumbers()` - Floating damage text animation
- `renderDamageNumbers()` - Render floating damage text
- `renderPlayerUI()` - Render HUD (HP, XP, Gold, Items)

### 3. Input System

**InputManager** (`InputManager.h`):
- Context-aware input binding (e.g., "Gameplay", "Menu")
- Action-based input (e.g., "Move", "Attack", "Interact")
- 2D axis binding (WASD/Arrow keys)
- Key binding with modifiers
- Input context switching

**Usage Example:**
```cpp
m_inputManager->bind2DAxis("Gameplay", "Move", SDLK_W, SDLK_S, SDLK_A, SDLK_D);
m_inputManager->bindKey("Gameplay", "Attack", SDLK_SPACE);
m_inputManager->setActiveContext("Gameplay");
```

### 4. Scene System

**Scene Base Class** (`Scene.h`):
- Virtual methods: `onInit()`, `onUpdate(dt)`, `onRender()`, `onShutdown()`
- Scene lifecycle management
- Input handling per scene

**SceneManager** (`SceneManager.h`):
- Scene stack management
- Scene transitions
- Active scene tracking

**Available Scenes:**
- `MenuScene` - Main menu with title and start button
- `GameScene` - Main gameplay scene
- `PauseScene` - Pause menu overlay

### 5. Resource Management

**ResourceManager** (`ResourceManager.h`):
- YAML-based sprite sheet loading
- Texture loading from files
- Resource manifest parsing
- Sprite frame extraction from sprite sheets

**Sprite Sheet Format** (YAML):
```yaml
sprites:
  - name: "grass_tile"
    x: 0
    y: 0
    width: 16
    height: 16
```

### 6. Camera System

**Camera** (`Camera.h`):
- World-to-screen coordinate transformation
- Screen-to-world coordinate transformation
- Smooth following with configurable smoothing
- Zoom support (0.25x to 4.0x)
- Manual camera controls (WASD movement)
- Mouse panning support
- World bounds calculation for culling

**Usage:**
```cpp
camera.followEntity(registry, playerEntity, 0.1f); // Smooth follow
camera.worldToScreen(worldX, worldY, screenX, screenY);
```

### 7. Font Rendering

**Font** (`Font.h`):
- SDL3_ttf integration
- Text rendering to textures
- Color support (SDL_Color)
- Transparent background rendering

**API:**
```cpp
auto texture = font->renderText("Hello World", {255, 255, 255, 255});
spriteBatch->draw(*texture, x, y);
```

**Note:** SDL3_ttf `TTF_RenderText_Blended` signature:
```cpp
TTF_RenderText_Blended(font, text, length, color)
```

### 8. Dear ImGui Integration (NEW)

**ImGuiManager** (`ImGui.h`):
- C++ wrapper for Dear ImGui
- Vulkan backend integration
- Manual SDL3 event processing (SDL3 backend disabled due to API incompatibilities)
- Frame setup and rendering

**Status:**
- ✅ ImGui context initialization
- ✅ Manual SDL3 event handling (mouse, keyboard, text input)
- ✅ Frame setup (display size, delta time)
- ⚠️ Vulkan rendering integration pending (requires Vulkan2D render pass integration)

**Usage:**
```cpp
ImGuiManager imgui(window, renderer);
imgui.beginFrame();
ImGui::Text("Hello, world!");
imgui.endFrame();
```

---

## Entry Points

All game entry points are in the `Sandbox/` directory:

### 1. `Sandbox/main_rpg.cpp` (PRIMARY - RPG Demo)
- Full RPG gameplay demo
- Player movement, combat, inventory
- Enemy AI, quest system
- UI rendering (HP, XP, Gold, Items)
- Damage numbers, item collection

### 2. `Sandbox/main.cpp` (Basic ECS Demo)
- Simple ECS demo with player movement
- Grass tile rendering
- Basic camera following

### 3. `Sandbox/main_input_demo.cpp` (Input System Demo)
- Input binding system demonstration
- Context switching
- Action-based input

### 4. `Sandbox/main_scene_demo.cpp` (Scene System Demo)
- Scene management demonstration
- Scene transitions

---

## Build Configuration

### CMake Configuration
- **C++ Standard:** C++20 (required, no extensions)
- **Build Type:** Debug (`-g -O0`) or Release (`-O3`)
- **Architecture:** Shared library (`Runa2Engine`) + executable (`Runa2`)
- **Generator:** Ninja
- **Compile Commands:** Exported for clangd/IntelliSense

### Build Commands
```bash
# Debug build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug

# Release build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release
```

### First Build
- Takes 5-15 minutes (downloads and compiles all dependencies)
- Subsequent builds are incremental and fast

---

## Current Demo: RPG Game

**Features:**
- 50x50 tile world with grass, dirt, and rock tiles
- Player character (blue rectangle) with WASD movement
- 8 enemy slimes (green rectangles) with AI (chase, attack, patrol)
- 10 health potions and 10 gold coins (red/yellow rectangles)
- Quest giver NPC (purple rectangle)
- Combat system (player attacks enemies, enemies attack player)
- Experience and leveling system
- Inventory system (20 slots)
- Damage numbers (floating text)
- UI display (HP, Level, XP, Gold, Items)
- Camera following player smoothly

**Controls:**
- **WASD** - Move player
- **SPACE** - Attack (auto-attacks nearby enemies)
- **I** - Toggle inventory
- **E** - Interact with NPCs
- **ESC** - Quit

---

## Known Issues & Limitations

### Fixed Issues
- ✅ Font rendering bug (incorrect `TTF_RenderText_Blended` parameters) - **FIXED**
- ✅ Entity rendering (entities without sprite sheets not rendering) - **FIXED**
- ✅ Tile rendering (tiles not being drawn) - **FIXED**
- ✅ Camera initialization (camera not positioned at player start) - **FIXED**

### Current Limitations
- ⚠️ **ImGui Vulkan Rendering**: Vulkan backend rendering not yet integrated with Vulkan2D render passes
- ⚠️ **ResourceManager YAML Loading**: Complex YAML sprite sheet loading may cause crashes (workaround: direct texture loading)
- ⚠️ **Sprite Sheet Support**: Entities without sprite sheets render as colored rectangles (temporary solution)
- ⚠️ **SDL3 ImGui Backend**: Disabled due to API incompatibilities (using manual event handling)

### Performance
- **Target FPS:** 60 FPS (VSYNC-limited)
- **Resolution:** 1280x720
- **Rendering:** Hardware-accelerated via Vulkan2D
- **Batching:** Automatic sprite batching for optimal draw calls

---

## Development Patterns

### Application Lifecycle
```cpp
class MyGame : public Runa::Application {
protected:
    void onInit() override { /* Initialize resources */ }
    void onUpdate(float dt) override { /* Update game logic */ }
    void onRender() override { /* Render frame */ }
    void onShutdown() override { /* Cleanup */ }
};
```

### ECS Usage Pattern
```cpp
// Create entity
auto entity = registry->createEntity(x, y);
reg.emplace<Position>(entity, x, y);
reg.emplace<Size>(entity, width, height);
reg.emplace<Sprite>(entity);

// Update systems
Runa::ECS::Systems::updateMovement(reg, dt);
Runa::ECS::Systems::renderSprites(reg, *spriteBatch, *camera, whitePixelTexture);
```

### Rendering Pattern
```cpp
renderer.beginFrame();
spriteBatch->begin();

// Render world
renderWorld();

// Render entities
Runa::ECS::Systems::renderSprites(reg, *spriteBatch, *camera, whitePixelTexture);

// Render UI
Runa::ECS::RPGSystems::renderPlayerUI(reg, *spriteBatch, *font, width, height);

spriteBatch->end();
renderer.endFrame();
```

---

## Future Enhancements

### Planned Features
- Full ImGui Vulkan rendering integration
- Sprite animation system improvements
- Sound system integration
- Particle effects system
- Post-processing effects pipeline
- Resource hot-reloading
- Save/load system
- Networking support (future)

### Technical Debt
- Complete ImGui integration with Vulkan2D
- Fix ResourceManager YAML loading crashes
- Improve sprite sheet rendering for entities
- Add proper sprite animation support
- Optimize collision detection

---

## Contributing Guidelines

### Code Style
- C++20 standard (no extensions)
- RAII for resource management
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Const correctness
- Namespace: `Runa::` for engine code

### File Organization
- Headers in `src/` with matching `.cpp` files
- Precompiled header: `runapch.h`
- API exports: `RUNA_API` macro

### Logging
- Use `LOG_INFO()`, `LOG_DEBUG()`, `LOG_WARN()`, `LOG_ERROR()`, `LOG_CRITICAL()`
- Logging via spdlog wrapper

---

## Documentation

- **README.md** - Project overview and build instructions
- **CHANGELOG.md** - Version history and changes
- **REPOSITORY_ANALYSIS.md** - This file (comprehensive codebase overview)

---

## License

AGPLv3 License (see LICENSE.md)

---

## Author

soulwax@github

---

**Last Analysis:** January 2025
**Engine Version:** 1.0.0
**Status:** Active Development
