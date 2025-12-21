# Shaders

This directory contains GLSL shaders for the Runa2 game engine.

## Prerequisites

To compile shaders, you need the **Vulkan SDK** installed, which includes the `glslc` compiler.

Download from: https://vulkan.lunarg.com/sdk/home

## Compiling Shaders

### Windows
```bash
cd shaders
compile_shaders.bat
```

### Linux/Mac or Git Bash on Windows
```bash
cd shaders
chmod +x compile_shaders.sh
./compile_shaders.sh
```

## Shader Files

- `*.vert` - Vertex shaders (GLSL)
- `*.frag` - Fragment shaders (GLSL)
- `*.spv` - Compiled SPIR-V binaries (generated, gitignored)

## Example Shaders

- `basic.vert` / `basic.frag` - Simple pass-through shader with vertex colors

## Usage in Code

```cpp
auto shader = getRenderer().createShader(
    "shaders/basic.vert.spv",
    "shaders/basic.frag.spv"
);
```

## Writing New Shaders

1. Create `.vert` and `.frag` files with GLSL code (version 450)
2. Run the compilation script
3. The compiled `.spv` files will be generated
4. Load them in your application using the Shader class
