# Runa2 Architecture

This document describes the architecture and structure of the Runa2 game engine.

## Overview

Runa2 is a modern C++20 game engine built on SDL3's GPU API with support for GLSL shaders compiled to SPIR-V.

## Core Architecture

### Application Layer

```
main.cpp
    └── GameApp (extends Application)
            ├── Window
            ├── Renderer
            └── Game Loop
```

### Class Hierarchy

#### Application (`Application.h/cpp`)

The base application class that manages the game lifecycle.

**Responsibilities:**

- Initialize SDL3
- Create and manage Window and Renderer
- Run the main game loop
- Calculate delta time and FPS
- Provide virtual hooks for game logic

**Virtual Methods (Override in your game):**

- `onInit()` - Called once after initialization
- `onUpdate(float deltaTime)` - Called every frame for game logic
- `onRender()` - Called every frame for rendering
- `onShutdown()` - Called before cleanup

**Usage:**

```cpp
class MyGame : public Runa::Application {
public:
    MyGame() : Application("My Game", 1280, 720) {}

protected:
    void onInit() override {
        // Load resources, initialize game state
    }

    void onUpdate(float deltaTime) override {
        // Update game logic
    }

    void onRender() override {
        // Render the scene
        getRenderer().clear(0.0f, 0.0f, 0.0f, 1.0f);
    }
};
```

#### Window (`Window.h/cpp`)

Manages SDL3 window creation and event handling.

**Responsibilities:**

- Create and destroy SDL3 window
- Process window events (resize, close, keyboard)
- Track window state (size, should close)

**Key Features:**

- Resizable windows with automatic resize events
- ESC key to close
- Move semantics (no copying)

#### Renderer (`Renderer.h/cpp`)

Manages SDL3 GPU device and rendering operations.

**Responsibilities:**

- Initialize SDL3 GPU device
- Manage swapchain textures
- Clear screen
- Create shaders
- Manage render passes

**Key Methods:**

- `beginFrame()` - Acquire swapchain texture
- `endFrame()` - Submit command buffer and present
- `clear(r, g, b, a)` - Clear screen to color
- `createShader(vertPath, fragPath)` - Load compiled SPIR-V shaders

**Supported GPU Backends:**

- Direct3D 12 (Windows)
- Vulkan (Windows, Linux)

#### Shader (`Shader.h/cpp`)

Manages GLSL shaders compiled to SPIR-V format.

**Responsibilities:**

- Load SPIR-V binary files
- Create SDL3 GPU shader objects
- Manage shader lifecycle

**Workflow:**

1. Write GLSL shaders (`.vert`, `.frag`)
2. Compile to SPIR-V (`.spv`) using `glslc`
3. Load in engine using `Renderer::createShader()`

## Directory Structure

```
src/
├── main.cpp          - Entry point and GameApp class
├── Application.h/cpp - Base application class
├── Window.h/cpp      - Window management
├── Renderer.h/cpp    - SDL3 GPU renderer
└── Shader.h/cpp      - Shader management

shaders/
├── *.vert            - GLSL vertex shaders
├── *.frag            - GLSL fragment shaders
├── *.spv             - Compiled SPIR-V (gitignored)
└── compile_shaders.* - Compilation scripts

Resources/
├── Fonts/            - Font files
└── mystic_woods_2.2/ - Game assets
```

## Shader Pipeline

### 1. Write GLSL Shaders

Create vertex and fragment shaders using GLSL 450:

```glsl
// basic.vert
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor;
}
```

```glsl
// basic.frag
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}
```

### 2. Compile to SPIR-V

```bash
cd shaders
./compile_shaders.bat  # Windows
./compile_shaders.sh   # Linux/Mac
```

Requires **Vulkan SDK** with `glslc` compiler.

### 3. Load in Engine

```cpp
auto shader = getRenderer().createShader(
    "shaders/basic.vert.spv",
    "shaders/basic.frag.spv"
);
```

## Rendering Flow

```
Main Loop:
    1. Window::processEvents()
    2. Application::onUpdate(deltaTime)
    3. Renderer::beginFrame()
        └── Acquire swapchain texture
    4. Application::onRender()
        ├── Renderer::clear()
        └── Custom rendering
    5. Renderer::endFrame()
        └── Submit and present
```

## Frame Timing

The engine automatically tracks:

- Delta time (time between frames)
- FPS counter (printed every second)
- High-precision timing using `std::chrono`

## Event Handling

Currently supported events:

- **SDL_EVENT_QUIT** - Window close button
- **SDL_EVENT_WINDOW_RESIZED** - Window resize
- **SDL_EVENT_KEY_DOWN** - ESC key to close

Extend `Window::processEvents()` to handle more events.

## Memory Management

The codebase uses modern C++ RAII principles:

- **Unique pointers** for owned resources (Window, Renderer)
- **Shared pointers** for shared resources (Shader)
- **Move semantics** enabled, copying disabled
- **Automatic cleanup** via destructors

## Error Handling

The engine uses exceptions for initialization errors:

```cpp
try {
    auto app = std::make_unique<GameApp>();
    app->run();
} catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
}
```

Runtime errors are logged to `std::cerr`.

## Extending the Engine

### Adding New Systems

1. Create header/source files in `src/`
2. Add to `CMakeLists.txt`
3. Include in your `GameApp` class
4. Initialize in `onInit()`, update in `onUpdate()`, render in `onRender()`

### Example: Adding a Sprite System

```cpp
// src/SpriteRenderer.h
class SpriteRenderer {
    void draw(Texture& texture, vec2 position);
};

// In GameApp
class GameApp : public Runa::Application {
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;

    void onInit() override {
        m_spriteRenderer = std::make_unique<SpriteRenderer>(getRenderer());
    }

    void onRender() override {
        m_spriteRenderer->draw(texture, {100, 100});
    }
};
```

## Performance Considerations

- **GPU backend**: Automatically selects D3D12 or Vulkan
- **Command buffers**: One per frame currently (can be extended)
- **Shader compilation**: Done offline (SPIR-V loaded at runtime)
- **Frame timing**: ~60 FPS target (no hard vsync limit)

## Dependencies

- **SDL3**: Window, events, GPU abstraction
- **C++20**: Modern C++ features
- **Vulkan SDK**: For shader compilation only (not runtime)

## Future Enhancements

Potential areas for expansion:

- Texture loading and management
- Sprite batching system
- Camera and transformation matrices
- Input management system
- Audio system
- Scene graph
- Entity-Component-System (ECS)
- Physics integration
