# Sandbox

This directory contains the **game implementation** code that uses the Runa2 engine.

## What Goes Here

- **Game entry points** (`main*.cpp` files)
- **Game-specific classes** that inherit from engine classes
- **Game logic** that uses the engine API

## What Stays in `src/`

- **Engine code** (everything in the `Runa::` namespace)
- **Core systems** (Application, Input, Renderer, etc.)
- **ECS components and systems** (in `Runa::ECS` namespace)
- **Graphics systems** (Camera, SpriteBatch, Texture, etc.)
- **Scene base classes** (Scene, SceneManager)

## Include Paths

All Sandbox files should include engine headers using relative paths from `src/`:

```cpp
#include "Core/Application.h"
#include "Graphics/SpriteBatch.h"
#include "ECS/Components.h"
```

The CMake build system automatically adds `src/` to the include path for Sandbox executables.

## Current Files

- `main_rpg.cpp` - RPG demo game (PRIMARY)
- `main.cpp` - Basic ECS demo
- `main_input_demo.cpp` - Input system demo
- `main_scene_demo.cpp` - Scene system demo
