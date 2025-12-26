# Shaders

This directory contains GLSL shaders for the Runa2 game engine.

## Prerequisites

To compile shaders, you need the **Vulkan SDK** installed, which includes the `glslc` compiler.

Download from: <https://vulkan.lunarg.com/sdk/home>

## Compiling Shaders

The compile scripts are located in the project root directory.

### Windows

```bash
compile_shaders.bat
```

### Linux/Mac or Git Bash on Windows

```bash
chmod +x compile_shaders.sh
./compile_shaders.sh
```

## Shader Files

- `*.vert` - Vertex shaders (GLSL source)
- `*.frag` - Fragment shaders (GLSL source)
- `compiled/*.spv` - Compiled SPIR-V binaries (generated, gitignored)

## Available Shaders

### Basic Shaders

- `basic.vert` / `basic.frag` - Simple pass-through shader with vertex colors
- `sprite.vert` / `sprite.frag` - 2D sprite rendering with texture sampling and color tinting

### Post-Processing Shaders

All post-processing shaders use full-screen quads and require `screenSize` and `time` push constants.

- `psychedelic.vert` / `psychedelic.frag` - Trippy color cycling, wave distortion, and chromatic aberration effects
- `crt.vert` / `crt.frag` - Retro CRT monitor effect with scanlines, barrel distortion, and phosphor glow
- `pixelate.vert` / `pixelate.frag` - Pixelation effect for retro/pixel art style
- `bloom.vert` / `bloom.frag` - Bloom/glow effect that brightens and blurs bright areas
- `kaleidoscope.vert` / `kaleidoscope.frag` - Mirror/reflection effect creating symmetrical patterns
- `water.vert` / `water.frag` - Animated water ripple distortion with wave effects

## Usage in Code

```cpp
auto shader = getRenderer().createShader(
    "Resources/shaders/compiled/basic.vert.spv",
    "Resources/shaders/compiled/basic.frag.spv"
);
```

Note: The shader paths must point to the compiled `.spv` files in the `compiled/` directory.

## Writing New Shaders

1. Create `.vert` and `.frag` files with GLSL code (version 450) in this directory
2. Run the compilation script from the project root (`compile_shaders.bat` or `compile_shaders.sh`)
3. The compiled `.spv` files will be generated in the `compiled/` subdirectory
4. Load them in your application using the Shader class, pointing to the `.spv` files in `compiled/`
