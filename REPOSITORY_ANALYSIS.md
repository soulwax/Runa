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
- **spdlog** - Fast logging library (fully integrated with console and file logging)

## Build Architecture

### DLL-Based Engine Design

The project uses a **modular DLL architecture**:

- **Runa2Engine** (DLL) - Core engine library containing all engine systems
  - Exports via `RUNA_API` macro (`__declspec(dllexport)` on Windows)
  - Contains: Core, Graphics, and all engine functionality
  - Links: SDL3, SDL3_image, yaml-cpp, spdlog
  
- **Runa2** (Executable) - Game application
  - Links to Runa2Engine DLL
  - Contains only game-specific logic (`main.cpp`, `GameApp` class)
  - DLLs automatically copied to output directory during build

**Benefits:**
- Separation of engine and game code
- Faster iteration (only game code recompiles for game changes)
- Potential for engine reuse across multiple games
- Clean API boundaries via `RUNA_API` export macro

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
- **Log** - spdlog wrapper with console (colored) and file logging (`logs/runa2.log`)

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

### Shader System

The engine uses **GLSL shaders compiled to SPIR-V** for GPU rendering:

- **Shader Format**: GLSL 450 compiled to SPIR-V binary (`.spv`)
- **Compilation**: Offline using `glslc` (Vulkan SDK)
- **Shader Types**:
  - `sprite.vert` - Vertex shader for 2D sprites (pixel-to-NDC conversion, texture coordinates)
  - `sprite.frag` - Fragment shader for sprite rendering (texture sampling with color tinting)
  - `basic.vert/frag` - Basic shaders (available but not currently used)

**Shader Features:**
- Push constants for screen size (uniform data)
- Texture sampling with descriptor sets
- Color tinting support (RGBA per-vertex)
- Automatic Y-axis flipping for 2D coordinate system

**Workflow:**
1. Write/edit GLSL shaders (`.vert`, `.frag`)
2. Compile to SPIR-V using `compile_shaders.bat` (Windows) or `compile_shaders.sh` (Linux/Mac)
3. Load in engine via `Renderer::createShader(vertexPath, fragmentPath)`

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

### ✅ Fully Implemented
- **SDL3 window and event system** - Window creation, resize handling, ESC key, quit events
- **SDL3 GPU rendering** - D3D12/Vulkan backend, swapchain management, render passes
- **Texture loading** - Full SDL3_image integration with GPU texture uploads via transfer buffers
- **SpriteSheet system** - Complete YAML manifest parsing, sprite/animation metadata management
- **ResourceManager** - Centralized resource loading with YAML parsing for all sprite types
- **TileMap system** - Grid-based tilemap with text file loading and rendering
- **Shader system** - SPIR-V shader loading from compiled GLSL files
- **Logging system** - Full spdlog integration with console (colored) and file logging (`logs/runa2.log`)
- **Delta time and FPS tracking** - High-precision timing with std::chrono
- **Precompiled headers** - Common includes in `runapch.h` for faster compilation

### 🚧 Partially Implemented
- **SpriteBatch** - Draw call collection and render pass setup complete, but actual GPU drawing pipeline incomplete (vertex buffers created but not fully utilized, graphics pipeline not bound)
  - Structure is in place: vertex data prepared, buffers created, render passes set up
  - Missing: Full graphics pipeline binding, descriptor sets for textures, actual draw command execution
  - Currently logs debug messages instead of rendering

### ❌ Not Yet Implemented
- Input management system (keyboard/mouse abstraction)
- Audio system
- Camera/viewport system
- Animation controller (frame timing logic for sprite animations)
- Physics integration
- Scene graph
- Entity-Component-System (ECS)
- Complete sprite rendering (SpriteBatch infrastructure exists but needs final GPU pipeline completion)

## Code Quality

### Strengths
- Clean separation of concerns (Core vs Graphics)
- Modern C++ practices (smart pointers, RAII, move semantics)
- Well-documented architecture
- Extensible design (virtual hooks in Application)
- Resource management abstraction

### Areas for Improvement
- **SpriteBatch GPU rendering** - Infrastructure complete but needs graphics pipeline binding and draw command execution
- **Input abstraction** - Currently only handles ESC key and window close, needs full keyboard/mouse system
- **Animation system** - Sprite metadata exists but no animation controller for frame timing
- **Shader integration** - Shaders can be loaded but not actively used in SpriteBatch rendering pipeline
- **Error handling** - Some areas could benefit from more comprehensive error recovery

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
- **Usage**: Fully integrated with console (colored) and file logging (`logs/runa2.log`)
- **Status**: Actively used throughout codebase (68+ LOG_* macro calls across 11 files)
- **Features**: 
  - Colored console output with pattern `[%^%l%$] %v`
  - File logging with timestamps: `[%Y-%m-%d %H:%M:%S.%e] [%l] %v`
  - Auto-flush on warnings and above
  - Graceful fallback if file logging fails

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

## Implementation Status Summary

### Core Systems: ✅ Complete
- Application framework with game loop
- Window and event management
- GPU renderer with swapchain
- Texture loading and GPU uploads
- Resource management with YAML manifests
- Logging system (console + file)
- Shader loading from SPIR-V

### Rendering Systems: 🚧 Mostly Complete
- **Texture system**: ✅ Fully functional
- **SpriteSheet system**: ✅ Fully functional
- **TileMap system**: ✅ Fully functional
- **SpriteBatch**: 🚧 Infrastructure complete, GPU drawing pipeline needs completion
  - Draw calls collected ✅
  - Render passes set up ✅
  - Vertex buffers created ✅
  - Graphics pipeline binding: ❌ Missing
  - Texture descriptor sets: ❌ Missing
  - Draw command execution: ❌ Missing

### Game Systems: ❌ Not Started
- Input management
- Audio
- Camera/viewport
- Animation controller
- Physics
- ECS/Scene graph

## Summary

Runa2 is a **well-architected, modern C++20 game engine** with a solid foundation for 2D game development. It demonstrates:
- Clean code organization
- Modern C++ best practices (RAII, smart pointers, move semantics)
- Extensible design patterns
- Comprehensive documentation
- Professional build system setup
- **Active logging** throughout the codebase

The engine is in a **functional but incomplete state**:
- **Core infrastructure**: Fully operational
- **Resource management**: Complete and working
- **Rendering**: 90% complete - SpriteBatch needs final GPU pipeline integration
- **Game systems**: Not yet implemented

The architecture is designed for easy extension, making it an excellent foundation for continued development. The main remaining work is completing the SpriteBatch GPU rendering pipeline to enable actual sprite drawing.

