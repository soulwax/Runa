# Runa2 Documentation

Welcome to the Runa2 game engine documentation.

## Getting Started

- **[CLAUDE.md](../CLAUDE.md)** - Complete engine overview, build system, and development workflow
- **[CHANGELOG.md](../CHANGELOG.md)** - Version history and release notes

## Technical Guides

### Rendering System
- **[VK2D_INTEGRATION.md](VK2D_INTEGRATION.md)** - Comprehensive guide to our Vulkan2D renderer
  - Architecture and integration layer
  - Complete API reference
  - Performance benchmarks and optimization tips
  - Advanced features (shaders, cameras, render targets)
  - Troubleshooting guide

## Quick Links

### VK2D (Vulkan2D Renderer)
**Location:** `vendor/Vulkan2D/` (Git submodule)
**Status:** Integrated as part of our codebase (modifiable)
**Version:** 0.8.0

VK2D is our hardware-accelerated 2D rendering backend. It's not just a dependency—it's part of our codebase and we can modify it as needed.

**Key Features:**
- Automatic sprite batching
- Multi-camera system (up to 10 cameras)
- Custom SPIR-V shader support
- Render-to-texture capabilities
- 2D shadow rendering
- Nuklear GUI integration

**Our Integration:**
- C++ wrappers in `src/Graphics/` (Renderer, Texture, SpriteBatch)
- RAII memory management
- Type-safe APIs
- Integrated with SDL3 window system

**Performance:**
- 3,600 tiles @ 59-61 FPS (RTX 3070 Ti)
- VSync-limited, no CPU bottleneck
- Automatic batching for optimal draw calls

**See:** [VK2D_INTEGRATION.md](VK2D_INTEGRATION.md) for complete details

### ECS (Entity Component System)
**Library:** EnTT v3.13.2
**Location:** `src/ECS/`

**Components:**
- Position, Velocity, Size
- Sprite, Animation
- AABB, CollisionLayer
- PlayerInput, CameraTarget
- Tags: Player, Enemy, Projectile, etc.

**Systems:**
- updatePlayerInput - Input handling
- updateMovement - Physics integration
- updateAnimation - Frame updates
- updateTileCollisions - Collision resolution
- renderSprites - Sprite rendering
- updateCameraFollow - Camera tracking

**See:** [CLAUDE.md](../CLAUDE.md) - Entity Component System section

### Graphics Pipeline
**Backend:** Vulkan2D
**Wrapper:** `src/Graphics/`

**Core Classes:**
- `Renderer` - Frame lifecycle, VK2D wrapper
- `Texture` - Texture loading/management
- `SpriteBatch` - Automatic batched rendering
- `Camera` - 2D camera with smooth following
- `TileMap` - Grid-based tile rendering
- `Font` - TrueType text rendering

**Rendering Flow:**
```cpp
void onRender() override {
    // Note: beginFrame() called by Application::mainLoop()
    getRenderer().clear(r, g, b, a);

    m_spriteBatch->begin();
    m_spriteBatch->draw(texture, x, y, srcX, srcY, w, h);
    m_spriteBatch->end();

    // Note: endFrame() called by Application::mainLoop()
}
```

### Build System
**Generator:** Ninja
**Compiler:** GCC/MinGW (C++20)
**Platform:** Windows

**Commands:**
```bash
# Configure
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -S . -B build/debug

# Build
cmake --build build/debug

# Run
build/debug/Runa2.exe

# VSCode: Just press F5 (builds and debugs automatically)
```

**See:** [CLAUDE.md](../CLAUDE.md) - Build System section

## Project Status

**Current Version:** 0.1.0 (Initial Release)
**Last Updated:** 2026-01-03

**Working:**
- ✅ Vulkan2D rendering at 60 FPS
- ✅ ECS architecture with player movement
- ✅ Sprite sheet rendering
- ✅ Grass tile demo (3,600 tiles)
- ✅ Input system (Arrow keys/WASD)

**Known Issues:**
- ⚠️ ResourceManager YAML loading causes crashes
- ⚠️ Per-sprite color tinting not implemented
- ⚠️ Rotation/origin not exposed in SpriteBatch API

**Planned:**
- 🔲 Fix YAML loading
- 🔲 Camera following system
- 🔲 Tile-based collision
- 🔲 Enemy entities
- 🔲 Animation system
- 🔲 Sound system

**See:** [CHANGELOG.md](../CHANGELOG.md) for complete history

## Development Notes

### VK2D is Part of Our Codebase

**Important:** VK2D is integrated as a Git submodule and should be treated as part of our codebase, not just an external dependency.

**This means:**
- ✅ We can modify VK2D source code
- ✅ We can add features to VK2D
- ✅ We can fix bugs in VK2D
- ✅ We understand VK2D internals

**VK2D Source:**
- 23 C files (~350KB total)
- Well-documented with extensive comments
- Clean architecture with clear separation

**Key Files:**
- `Renderer.c` (47KB) - Main rendering loop
- `RendererMeta.c` (91KB) - Pipeline management
- `DescriptorBuffer.c` - VRAM memory management
- `Texture.c` - Texture loading
- `Image.c` - Image processing

### Critical Rendering Bug (FIXED)

**Bug:** Double-calling `beginFrame()`/`endFrame()` causes silent crashes
**Location:** Application.cpp already calls these in mainLoop() (lines 113-115)
**Fix:** Don't call `beginFrame()`/`endFrame()` in `onRender()`
**See:** [CHANGELOG.md](../CHANGELOG.md) v0.1.0 - Fixed section

### Precompiled Headers

**File:** `src/runapch.h`
**Issue:** spdlog include order matters
**Order:** `spdlog.h` → `basic_file_sink.h` → `stdout_color_sinks.h`
**See:** [CHANGELOG.md](../CHANGELOG.md) v0.1.0 - Fixed section

## Contributing

When modifying the codebase:

1. **Update Documentation**
   - Update CLAUDE.md for engine changes
   - Update VK2D_INTEGRATION.md for VK2D changes
   - Add entry to CHANGELOG.md

2. **Follow Existing Patterns**
   - Use RAII for resource management
   - Prefer ECS over legacy Entity class
   - Use LOG_* macros for logging

3. **Test Thoroughly**
   - Test on target hardware (RTX 3070 Ti in our case)
   - Check FPS and VRAM usage
   - Verify VSync behavior

4. **Commit Messages**
   - Format: `type(scope): description`
   - Include co-author for Claude assistance

## Resources

- **VK2D GitHub:** [PaoloMazzon/Vulkan2D](https://github.com/PaoloMazzon/Vulkan2D)
- **EnTT GitHub:** [skypjack/entt](https://github.com/skypjack/entt)
- **SDL3 GitHub:** [libsdl-org/SDL](https://github.com/libsdl-org/SDL)
- **Vulkan Docs:** [vulkan.org](https://www.vulkan.org/)

---

**For detailed information, see the specific documentation files listed above.**
