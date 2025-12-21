# Project Structure

This document outlines the organization of the Runa2 game engine codebase.

## Source Code Organization

```
src/
├── main.cpp              # Application entry point
├── Core/                 # Core engine systems
│   ├── Application.h
│   └── Application.cpp   # Base application framework
└── Graphics/             # Graphics and rendering systems
    ├── Window.h
    ├── Window.cpp        # SDL3 window management
    ├── Renderer.h
    ├── Renderer.cpp      # SDL3 GPU renderer
    ├── Shader.h
    └── Shader.cpp        # GLSL/SPIR-V shader loading
```

## Directory Purposes

### `src/Core/`
Core engine functionality that doesn't depend on specific subsystems.

**Current Classes:**
- **Application** - Game lifecycle management, main loop, delta time calculation

**Future additions might include:**
- Time management
- Logging system
- Configuration/settings
- Resource management base classes

### `src/Graphics/`
All graphics and rendering-related code.

**Current Classes:**
- **Window** - SDL3 window creation and event handling
- **Renderer** - SDL3 GPU device and rendering operations
- **Shader** - GLSL shader compilation and loading

**Future additions might include:**
- Texture loading and management
- Sprite rendering
- Camera systems
- Mesh/Model loading
- Material systems
- Particle systems

## Adding New Modules

When adding new engine systems, create appropriate subdirectories:

### Suggested Future Organization

```
src/
├── main.cpp
├── Core/
│   ├── Application.*
│   ├── Time.*
│   ├── Logger.*
│   └── Config.*
├── Graphics/
│   ├── Window.*
│   ├── Renderer.*
│   ├── Shader.*
│   ├── Texture.*
│   ├── Sprite.*
│   └── Camera.*
├── Audio/
│   ├── AudioEngine.*
│   ├── Sound.*
│   └── Music.*
├── Input/
│   ├── InputManager.*
│   ├── Keyboard.*
│   └── Mouse.*
├── Physics/
│   ├── PhysicsWorld.*
│   ├── Collider.*
│   └── RigidBody.*
└── Game/
    ├── Entity.*
    ├── Component.*
    └── Scene.*
```

## Include Conventions

### From main.cpp or game code:
```cpp
#include "Core/Application.h"
#include "Graphics/Renderer.h"
```

### From Core files:
```cpp
// Application.h
#include "../Graphics/Window.h"
#include "../Graphics/Renderer.h"
```

### Within same directory:
```cpp
// Renderer.h in Graphics/
#include "Window.h"
#include "Shader.h"
```

## CMakeLists.txt Organization

Files are grouped by module in CMakeLists.txt:

```cmake
add_executable(Runa2
    src/main.cpp

    # Core
    src/Core/Application.cpp
    src/Core/Application.h

    # Graphics
    src/Graphics/Window.cpp
    src/Graphics/Window.h
    src/Graphics/Renderer.cpp
    src/Graphics/Renderer.h
    src/Graphics/Shader.cpp
    src/Graphics/Shader.h
)
```

## Build System

The `src/` directory is added to include paths, allowing:
- Clean includes from main.cpp: `#include "Core/Application.h"`
- Relative includes within modules

See [CMakeLists.txt](CMakeLists.txt) for the full build configuration.

## File Naming Conventions

- **Headers**: `ClassName.h`
- **Implementation**: `ClassName.cpp`
- **One class per file** (recommended)
- **File names match class names**

## Namespace

All engine code is in the `Runa` namespace:

```cpp
namespace Runa {
    class Application { ... };
    class Renderer { ... };
}
```

## Adding New Files Checklist

1. Create header and source files in appropriate `src/SubModule/` directory
2. Add both files to `CMakeLists.txt` under the correct module comment
3. Use `#pragma once` for header guards
4. Place class in `Runa` namespace
5. Follow include conventions based on file location
6. Rebuild to verify everything compiles

## Additional Resources

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed architecture documentation.
See [CLAUDE.md](CLAUDE.md) for build and development workflow.
