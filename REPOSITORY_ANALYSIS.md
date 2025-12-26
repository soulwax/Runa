# Runa2 Repository Analysis

**Generated:** 2024  
**Project Type:** C++20 2D Game Engine  
**License:** AGPLv3

---

## Executive Summary

**Runa2** is a modern C++20 2D game engine built on SDL3's GPU API. It provides a modular, extensible foundation for 2D game development with hardware-accelerated rendering, sprite management, tilemaps, and a declarative resource system. The engine uses a DLL-based architecture separating engine code from game logic, enabling rapid iteration and code reuse.

**Current Status:** Core infrastructure is complete and functional. Rendering pipeline is ~90% complete with SpriteBatch needing final GPU pipeline integration. Game systems (input, audio, physics) are not yet implemented.

---

## Project Overview

### Purpose
A professional-grade 2D game engine framework designed for:
- 2D sprite-based games
- Tilemap-based level rendering
- Hardware-accelerated graphics (D3D12/Vulkan via SDL3 GPU)
- Rapid game development with declarative resource management

### Target Platform
- **Primary:** Windows (MinGW/GCC)
- **Graphics Backends:** Direct3D 12, Vulkan (via SDL3 GPU abstraction)
- **Build System:** CMake 3.20+ with Ninja generator

---

## Technology Stack

### Core Technologies
- **Language:** C++20 (strict standard, no extensions)
- **Compiler:** GCC/G++ (MinGW) on Windows
- **Build System:** CMake 3.20+ with Ninja generator
- **Graphics API:** SDL3 GPU (abstraction over D3D12/Vulkan)
- **Shaders:** GLSL 450 compiled to SPIR-V (offline compilation)

### Dependencies (via CMake FetchContent)

| Dependency | Version | Purpose | Status |
|------------|---------|---------|--------|
| **SDL3** | main branch | Window management, events, GPU abstraction | ✅ Fully integrated |
| **SDL3_image** | main branch | Image loading (PNG support) | ✅ Fully integrated |
| **SDL3_ttf** | main branch | TrueType font rendering | ✅ Fully integrated |
| **yaml-cpp** | master branch | YAML parsing for sprite manifests | ✅ Fully integrated |
| **spdlog** | v1.15.0 | Fast logging library | ✅ Fully integrated |

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
    ├── Renderer (Graphics) - SDL3 GPU device & rendering
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

#### 2. Graphics Module (`src/Graphics/`)

| Class | Purpose | Status |
|-------|---------|--------|
| **Window** | SDL3 window creation, event processing | ✅ Complete |
| **Renderer** | SDL3 GPU device, swapchain, frame rendering | ✅ Complete |
| **Shader** | SPIR-V shader loading from compiled GLSL | ✅ Complete |
| **Texture** | Image loading via SDL3_image, GPU uploads | ✅ Complete |
| **SpriteSheet** | Texture atlas with sprite/animation metadata | ✅ Complete |
| **SpriteBatch** | 2D sprite batching system | 🚧 90% (needs GPU pipeline completion) |
| **TileMap** | Grid-based tilemap rendering | ✅ Complete |
| **PostProcess** | Post-processing effects framework | 🚧 Partial |
| **Font** | TrueType font rendering | ✅ Complete |

---

## File Structure

```
Runa2/
├── src/
│   ├── main.cpp              # Entry point, GameApp class
│   ├── runapch.h/cpp         # Precompiled header (common includes)
│   ├── RunaAPI.h             # DLL export/import macros
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
│       ├── TileMap.*         # Grid-based tilemap system
│       ├── PostProcess.*     # Post-processing effects
│       └── Font.*            # TrueType font rendering
│
├── Resources/
│   ├── Fonts/                # Font files (Renogare.otf)
│   ├── manifests/            # YAML sprite definitions (12 files)
│   │   ├── tilesets.yaml
│   │   ├── plains.yaml
│   │   ├── player.yaml
│   │   ├── slime.yaml
│   │   └── ... (8 more)
│   ├── scenes/               # Scene data files
│   └── mystic_woods_2.2/     # Game asset pack
│       └── sprites/          # Character & tile sprites
│
├── shaders/                  # GLSL shaders (compile to SPIR-V)
│   ├── sprite.vert/frag      # Sprite rendering shaders
│   ├── basic.vert/frag       # Basic shaders
│   ├── water.vert/frag       # Water effect shaders
│   ├── bloom.vert/frag       # Bloom post-process
│   ├── crt.vert/frag         # CRT effect
│   ├── pixelate.vert/frag    # Pixelation effect
│   ├── psychedelic.vert/frag # Psychedelic effect
│   ├── kaleidoscope.vert/frag# Kaleidoscope effect
│   └── compile_shaders.*     # Compilation scripts
│
├── build/                    # Build output (gitignored)
│   └── debug/                # Debug builds with symbols
│
├── vendor/                   # Third-party libraries (SDL3, etc.)
│   ├── SDL/                  # SDL3 source (fetched by CMake)
│   ├── SDL_image/            # SDL3_image source
│   ├── SDL_ttf/              # SDL3_ttf source
│   ├── yaml-cpp/             # yaml-cpp source
│   └── spdlog/               # spdlog source
│
├── CMakeLists.txt            # Build configuration
├── README.md                 # Build instructions
├── ARCHITECTURE.md           # Engine architecture docs
├── REPOSITORY_ANALYSIS.md    # This file
└── CLAUDE.md                 # AI assistant instructions
```

---

## Key Systems

### 1. Resource Management System

**YAML-Based Manifest System:**
- Declarative sprite definitions in YAML files
- Supports multiple sprite types: `single`, `animation`, `grid`, `frames`
- Automatic sprite generation from grid layouts
- Centralized loading via `ResourceManager`

**Example Manifest:**
```yaml
spritesheet:
  name: "plains_tileset"
  texture: "../mystic_woods_2.2/sprites/tilesets/plains.png"
  sprites:
    - name: "plains_tile"
      type: "grid"
      tile_width: 16
      tile_height: 16
      columns: 6
      rows: 12
```

**Features:**
- ✅ YAML parsing via yaml-cpp
- ✅ Sprite metadata (frames, animations, durations)
- ✅ Texture loading via SDL3_image
- ✅ GPU texture uploads via transfer buffers
- ✅ Sprite lookup by name

### 2. Rendering Pipeline

**Frame Rendering Flow:**
```
1. Renderer::beginFrame()     - Acquire swapchain texture
2. Renderer::clear()          - Set background color
3. SpriteBatch::begin()       - Start batching
4. SpriteBatch::draw()        - Queue draw calls
5. SpriteBatch::end()         - Flush to GPU (🚧 needs completion)
6. Renderer::endFrame()       - Submit and present
```

**Current Status:**
- ✅ Swapchain management
- ✅ Frame begin/end
- ✅ Clear operations
- ✅ Draw call collection
- ✅ Vertex buffer creation
- 🚧 Graphics pipeline binding (missing)
- 🚧 Texture descriptor sets (missing)
- 🚧 Draw command execution (missing)

### 3. Shader System

**Workflow:**
1. Write GLSL shaders (`.vert`, `.frag`) using GLSL 450
2. Compile to SPIR-V (`.spv`) using `glslc` (Vulkan SDK)
3. Load in engine via `Renderer::createShader()`

**Available Shaders:**
- `sprite.vert/frag` - 2D sprite rendering (pixel-to-NDC, texture sampling)
- `basic.vert/frag` - Basic shaders
- `water.vert/frag` - Water effects
- `bloom.vert/frag` - Bloom post-process
- `crt.vert/frag` - CRT effect
- `pixelate.vert/frag` - Pixelation
- `psychedelic.vert/frag` - Psychedelic warping
- `kaleidoscope.vert/frag` - Kaleidoscope effect

**Shader Features:**
- Push constants for screen size
- Texture sampling with descriptor sets
- Color tinting support (RGBA per-vertex)
- Automatic Y-axis flipping for 2D coordinates

### 4. Logging System

**spdlog Integration:**
- ✅ Console logging with colored output
- ✅ File logging to `logs/runa2.log` with timestamps
- ✅ Multiple log levels (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL)
- ✅ Auto-flush on warnings and above
- ✅ Graceful fallback if file logging fails
- ✅ 68+ LOG_* macro calls across 11 files

**Log Patterns:**
- Console: `[%^%l%$] %v` (colored level, message)
- File: `[%Y-%m-%d %H:%M:%S.%e] [%l] %v` (timestamp, level, message)

### 5. TileMap System

**Features:**
- ✅ Grid-based tilemap (configurable dimensions)
- ✅ Text file scene loading
- ✅ Tile index management
- ✅ SpriteSheet integration
- ✅ Centered rendering with offsets

**Current Demo:**
- 40x30 tile map (640x480 pixels at 16px/tile)
- Loads from `Resources/scenes/sample_scene.txt`
- Falls back to generated pattern if file missing

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
- **Module separation:** Core vs Graphics modules
- **Include conventions:** Relative includes within modules, absolute from main
- **Precompiled headers:** `runapch.h` for common includes

### API Design
- **DLL exports:** `RUNA_API` macro for Windows DLL exports
- **Virtual hooks:** Application class provides extensible game loop
- **Getter methods:** Protected accessors for Window and Renderer
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
2. First build compiles SDL3 from source (5-15 minutes)
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
| **GPU Renderer** | ✅ Complete | D3D12/Vulkan backend, swapchain |
| **Texture Loading** | ✅ Complete | SDL3_image integration, GPU uploads |
| **SpriteSheet System** | ✅ Complete | YAML parsing, sprite metadata |
| **ResourceManager** | ✅ Complete | Centralized resource loading |
| **TileMap System** | ✅ Complete | Grid-based rendering, scene loading |
| **Shader Loading** | ✅ Complete | SPIR-V shader loading |
| **Logging System** | ✅ Complete | spdlog integration (console + file) |
| **Font Rendering** | ✅ Complete | TrueType font support |

### 🚧 Partially Implemented

| System | Status | Notes |
|--------|--------|-------|
| **SpriteBatch** | 🚧 90% | Infrastructure complete, GPU pipeline needs completion |
| **PostProcess** | 🚧 Partial | Framework exists, swapchain texture issues |

**SpriteBatch Details:**
- ✅ Draw call collection
- ✅ Render pass setup
- ✅ Vertex buffer creation
- ✅ Shader loading
- ❌ Graphics pipeline binding
- ❌ Texture descriptor sets
- ❌ Draw command execution

### ❌ Not Yet Implemented

| System | Status | Notes |
|--------|--------|-------|
| **Input Management** | ❌ Not started | Only ESC key and window close handled |
| **Audio System** | ❌ Not started | No SDL3_audio integration |
| **Camera/Viewport** | ❌ Not started | No viewport transformation |
| **Animation Controller** | ❌ Not started | Sprite metadata exists, no timing logic |
| **Physics** | ❌ Not started | No physics integration |
| **Scene Graph** | ❌ Not started | No hierarchical scene management |
| **ECS** | ❌ Not started | No entity-component-system |

---

## Notable Features

### Strengths
1. **Clean Architecture** - Well-separated modules (Core vs Graphics)
2. **Modern C++** - Smart pointers, RAII, move semantics
3. **Extensible Design** - Virtual hooks in Application class
4. **Comprehensive Logging** - spdlog integration throughout
5. **Declarative Resources** - YAML-based sprite definitions
6. **Professional Build System** - CMake with proper dependency management
7. **DLL Architecture** - Separation of engine and game code
8. **Documentation** - Well-documented architecture and workflows

### Areas for Improvement
1. **SpriteBatch GPU Pipeline** - Needs graphics pipeline binding and draw commands
2. **Input Abstraction** - Currently minimal event handling
3. **Animation System** - Metadata exists but no controller
4. **Shader Integration** - Shaders loaded but not fully utilized in rendering
5. **Error Recovery** - Some areas could benefit from more comprehensive error handling

---

## Development Workflow

### Quick Start
1. **Build:** `F5` in VSCode or `cmake --build build/debug`
2. **Run:** Execute `build/debug/Runa2.exe`
3. **Debug:** Use VSCode debugger with breakpoints
4. **Iterate:** Incremental builds for rapid development

### Resource Workflow
1. **Create YAML manifest** in `Resources/manifests/`
2. **Load in game** via `ResourceManager::loadSpriteSheetFromYAML()`
3. **Access sprites** via `ResourceManager::getSpriteSheet()`

### Shader Workflow
1. **Edit GLSL** shaders in `shaders/`
2. **Compile** using `compile_shaders.bat` (requires Vulkan SDK)
3. **Load** in engine via `Renderer::createShader()`

---

## Dependencies Analysis

### SDL3
- **Purpose:** Window, events, GPU abstraction
- **Usage:** Core graphics and window management
- **Status:** ✅ Fully integrated
- **Backends:** D3D12 (Windows), Vulkan (Windows/Linux)

### SDL3_image
- **Purpose:** Image loading (PNG)
- **Usage:** Texture loading in ResourceManager
- **Status:** ✅ Fully integrated

### SDL3_ttf
- **Purpose:** TrueType font rendering
- **Usage:** Font class for text rendering
- **Status:** ✅ Fully integrated

### yaml-cpp
- **Purpose:** YAML parsing
- **Usage:** Sprite manifest loading
- **Status:** ✅ Fully integrated

### spdlog
- **Purpose:** Fast logging
- **Usage:** Logging throughout codebase
- **Status:** ✅ Fully integrated (68+ macro calls)
- **Features:** Console (colored) + file logging

---

## Code Quality Metrics

### Code Organization
- ✅ Clear module separation (Core vs Graphics)
- ✅ Consistent naming conventions
- ✅ Proper use of namespaces
- ✅ Precompiled headers for performance

### Modern C++ Practices
- ✅ Smart pointers (unique_ptr, shared_ptr)
- ✅ RAII for resource management
- ✅ Move semantics enabled
- ✅ Const correctness
- ✅ No raw pointers for ownership

### Documentation
- ✅ Architecture documentation (ARCHITECTURE.md)
- ✅ Build instructions (README.md)
- ✅ Code comments where needed
- ✅ AI assistant guidance (CLAUDE.md)

---

## Future Development Directions

### High Priority
1. **Complete SpriteBatch GPU Pipeline** - Enable actual sprite rendering
2. **Input Management System** - Keyboard/mouse abstraction
3. **Animation Controller** - Frame timing for sprite animations
4. **Camera/Viewport System** - Viewport transformations

### Medium Priority
5. **Shader Integration** - Full shader usage in rendering pipeline
6. **Audio System** - SDL3_audio integration
7. **Scene Management** - Scene graph or ECS architecture
8. **Post-Processing Fix** - Resolve swapchain texture issues

### Low Priority
9. **Physics Integration** - Physics engine integration
10. **Asset Pipeline** - Automated asset processing
11. **Scripting** - Lua or similar scripting support
12. **Networking** - Multiplayer support

---

## Summary

**Runa2** is a **well-architected, modern C++20 game engine** with a solid foundation for 2D game development. The engine demonstrates:

- ✅ **Professional code organization** with clear module separation
- ✅ **Modern C++ best practices** (RAII, smart pointers, move semantics)
- ✅ **Extensible design patterns** (virtual hooks, DLL architecture)
- ✅ **Comprehensive documentation** (architecture, build, workflow)
- ✅ **Active logging** throughout the codebase
- ✅ **Declarative resource system** (YAML manifests)

**Current State:**
- **Core Infrastructure:** ✅ Fully operational
- **Resource Management:** ✅ Complete and working
- **Rendering:** 🚧 90% complete - SpriteBatch needs final GPU pipeline integration
- **Game Systems:** ❌ Not yet implemented

The architecture is designed for easy extension, making it an excellent foundation for continued development. The main remaining work is completing the SpriteBatch GPU rendering pipeline to enable actual sprite drawing, followed by implementing game systems (input, audio, etc.).

---

**Analysis Date:** 2024  
**Repository:** Runa2  
**Author:** soulwax@github  
**License:** AGPLv3
