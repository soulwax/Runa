# Runa2 Repository Analysis

## Project Overview

**Runa2** is a modern C++20 2D game engine built on SDL3's GPU API. It provides a foundation for 2D game development with support for sprites, animations, tilemaps, and hardware-accelerated rendering using Direct3D 12 or Vulkan backends.

## Key Technologies

### Core Stack
- **Language**: C++20 (strict standard, no extensions)
- **Build System**: CMake 3.20+ with Ninja generator
- **Compiler**: GCC/G++ (MinGW) on Windows
- **Graphics API**: SDL3 GPU (abstraction over D3D12/Vulkan)
- **Shaders**: GLSL compiled to SPIR-V (offline compilation)

### Dependencies (via CMake FetchContent)
- **SDL3** - Window management, events, GPU abstraction
- **SDL3_image** - Image loading (PNG support)
- **yaml-cpp** - YAML parsing for sprite manifests
- **spdlog** - Fast logging library (integrated but not fully utilized)

## Architecture

### Application Framework

The engine follows a classic game loop pattern with a base `Application` class:

```
Application (Core)
    ├── Window (Graphics) - SDL3 window & events
    ├── Renderer (Graphics) - SDL3 GPU device & rendering
    └── Game Loop
        ├── onInit() - One-time initialization
        ├── onUpdate(deltaTime) - Per-frame game logic
        ├── onRender() - Per-frame rendering
        └── onShutdown() - Cleanup
```

### Core Systems

#### 1. **Core Module** (`src/Core/`)
- **Application** - Main game loop, delta time calculation, FPS tracking
- **ResourceManager** - Centralized resource loading and management
- **Log** - spdlog wrapper (defined but minimal usage)

#### 2. **Graphics Module** (`src/Graphics/`)
- **Window** - SDL3 window creation, event processing, resize handling
- **Renderer** - SDL3 GPU device management, frame rendering, swapchain
- **Shader** - SPIR-V shader loading (GLSL compiled offline)
- **Texture** - Image loading via SDL3_image, GPU texture upload
- **SpriteSheet** - Texture atlas management with sprite/animation metadata
- **SpriteBatch** - 2D sprite batching system for efficient rendering
- **TileMap** - Grid-based tilemap rendering system

### Resource Management

The engine uses a **YAML-based manifest system** for declarative sprite definitions:

```yaml
spritesheet:
  name: "plains_tileset"
  texture: "../path/to/texture.png"
  sprites:
    - name: "plains_tile"
      type: "grid"
      tile_width: 16
      tile_height: 16
      columns: 6
      rows: 12
```

**Supported sprite types:**
- **Single** - Static sprites
- **Animation** - Multi-frame sequences with timing
- **Grid** - Auto-generated tilesets
- **Frames** - Manual frame definitions

### Rendering Pipeline

1. **Frame Begin**: `Renderer::beginFrame()` - Acquires swapchain texture
2. **Clear**: `Renderer::clear()` - Sets background color
3. **Sprite Batching**: `SpriteBatch::begin()` → `draw()` calls → `end()`
4. **Frame End**: `Renderer::endFrame()` - Submits and presents

### Current Demo

The `main.cpp` implements a **Tilemap Demo**:
- Loads a plains tileset from YAML manifest
- Creates a 40x30 tile map (640x480 pixels at 16px/tile)
- Loads scene data from text file or generates fallback pattern
- Renders centered tilemap using SpriteBatch

## File Structure

```
Runa2/
├── src/
│   ├── main.cpp              # Entry point, GameApp class
│   ├── runapch.h/cpp         # Precompiled header (common includes)
│   ├── Core/
│   │   ├── Application.*     # Base application framework
│   │   ├── ResourceManager.* # Resource loading & management
│   │   └── Log.*             # Logging wrapper (spdlog)
│   └── Graphics/
│       ├── Window.*          # SDL3 window management
│       ├── Renderer.*        # SDL3 GPU renderer
│       ├── Shader.*          # SPIR-V shader loading
│       ├── Texture.*         # Image loading & GPU textures
│       ├── SpriteSheet.*     # Texture atlas & sprite metadata
│       ├── SpriteBatch.*     # 2D sprite batching
│       └── TileMap.*         # Grid-based tilemap system
├── Resources/
│   ├── Fonts/                # Font files
│   ├── manifests/            # YAML sprite definitions
│   │   ├── player.yaml
│   │   ├── slime.yaml
│   │   └── tilesets.yaml
│   ├── scenes/               # Scene data files
│   └── mystic_woods_2.2/     # Game asset pack
│       └── sprites/          # Character & tile sprites
├── shaders/                  # GLSL shaders (compile to SPIR-V)
│   ├── basic.vert
│   ├── basic.frag
│   └── compile_shaders.*     # Compilation scripts
├── build/                    # Build output (gitignored)
│   └── debug/                # Debug builds with symbols
├── CMakeLists.txt            # Build configuration
└── Documentation/
    ├── README.md             # Build instructions
    ├── ARCHITECTURE.md       # Engine architecture
    ├── STRUCTURE.md          # Code organization
    ├── SPRITESHEET_SYSTEM.md # Sprite system guide
    └── CLAUDE.md             # Development workflow
```

## Design Patterns

### Memory Management
- **RAII** - All resources managed via smart pointers
- **Unique pointers** - Owned resources (Window, Renderer, ResourceManager)
- **Shared pointers** - Shared resources (Shader)
- **Move semantics** - Enabled for all graphics classes
- **No copying** - Graphics classes are non-copyable

### Error Handling
- **Exceptions** - Used for initialization failures
- **Runtime errors** - Logged to `std::cerr`
- **Null checks** - Resource lookups return pointers (nullptr on failure)

### Code Organization
- **Namespace**: All engine code in `Runa` namespace
- **Module separation**: Core vs Graphics modules
- **Include conventions**: Relative includes within modules, absolute from main

## Build Configuration

### Compiler Flags
- **Debug**: `-g -O0` (full symbols, no optimization)
- **Release**: `-O3` (maximum optimization)
- **C++ Standard**: C++20 (required, no extensions)

### Build Process
1. CMake configures project and fetches dependencies
2. First build compiles SDL3 from source (5-15 minutes)
3. Subsequent builds are incremental
4. Generates `compile_commands.json` for IntelliSense

### Development Tools
- **VSCode** - Fully configured with tasks and launch configs
- **F5** - Build and debug
- **Ctrl+Shift+B** - Build only
- **clangd** - Language server support via compile commands

## Notable Features

### ✅ Implemented
- SDL3 window and event system
- SDL3 GPU rendering (D3D12/Vulkan)
- Texture loading via SDL3_image
- SpriteSheet system with YAML manifests
- SpriteBatch for efficient 2D rendering
- TileMap system for grid-based maps
- ResourceManager for centralized loading
- Delta time and FPS tracking
- Precompiled headers for faster compilation

### 🚧 Partially Implemented
- **Logging** - spdlog integrated but minimal usage (macros defined)
- **Shaders** - Infrastructure exists but not actively used in current demo

### ❌ Not Yet Implemented
- Input management system
- Audio system
- Camera/viewport system
- Animation controller (frame timing logic)
- Physics integration
- Scene graph
- Entity-Component-System (ECS)
- Sprite rendering pipeline (SpriteBatch exists but needs GPU implementation)

## Code Quality

### Strengths
- Clean separation of concerns (Core vs Graphics)
- Modern C++ practices (smart pointers, RAII, move semantics)
- Well-documented architecture
- Extensible design (virtual hooks in Application)
- Resource management abstraction

### Areas for Improvement
- Logging system not fully utilized (spdlog available but minimal usage)
- Shader system infrastructure exists but not integrated into rendering
- SpriteBatch has draw call collection but actual GPU rendering not implemented
- No input abstraction layer
- Limited error handling in some areas

## Dependencies Analysis

### SDL3
- **Purpose**: Window, events, GPU abstraction
- **Usage**: Core graphics and window management
- **Status**: Fully integrated

### SDL3_image
- **Purpose**: Image loading (PNG)
- **Usage**: Texture loading in ResourceManager
- **Status**: Fully integrated

### yaml-cpp
- **Purpose**: YAML parsing
- **Usage**: Sprite manifest loading
- **Status**: Fully integrated

### spdlog
- **Purpose**: Fast logging
- **Usage**: Log.h wrapper exists, but minimal actual usage
- **Status**: Infrastructure ready, underutilized

## Development Workflow

1. **Edit code** in `src/`
2. **Build** via F5 (VSCode) or `cmake --build build/debug`
3. **Debug** with breakpoints in VSCode
4. **Test** by running executable

The project is configured for rapid iteration with incremental builds and automatic IntelliSense.

## Future Development Directions

Based on the architecture and documentation, likely next steps:
1. Complete SpriteBatch GPU rendering implementation
2. Add input management system
3. Implement animation controller for sprite animations
4. Add camera/viewport system
5. Integrate shader system into rendering pipeline
6. Expand logging usage throughout codebase
7. Add audio system (SDL3_audio)
8. Consider ECS architecture for game objects

## Summary

Runa2 is a **well-architected, modern C++20 game engine** with a solid foundation for 2D game development. It demonstrates:
- Clean code organization
- Modern C++ best practices
- Extensible design patterns
- Comprehensive documentation
- Professional build system setup

The engine is in an **early but functional state** - core systems are in place, but some rendering features need completion. The architecture is designed for easy extension, making it a good foundation for continued development.

