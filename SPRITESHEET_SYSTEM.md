# Spritesheet System Documentation

Comprehensive guide to using the Runa2 spritesheet and resource management system.

## Overview

The spritesheet system provides an efficient way to load, manage, and render 2D sprites from texture atlases. It supports:

- **Static sprites** - Single frame images
- **Animations** - Multi-frame sequences
- **Tilesets** - Grid-based tile maps
- **YAML manifests** - Declarative sprite definitions
- **SDL3 GPU** - Hardware-accelerated texture uploads

## Architecture

### Core Classes

```
ResourceManager
    ├── SpriteSheet (multiple)
    │   ├── Texture (GPU texture)
    │   └── Sprites (metadata)
    │       └── SpriteFrames
```

#### Texture ([src/Graphics/Texture.h](src/Graphics/Texture.h))
- Loads PNG images using SDL3_image
- Uploads to SDL3 GPU as RGBA8 textures
- Supports both file loading and raw pixel data

#### SpriteSheet ([src/Graphics/SpriteSheet.h](src/Graphics/SpriteSheet.h))
- Manages one texture atlas and its sprite definitions
- Defines sprite regions within the texture
- Supports animations with frame timing

#### ResourceManager ([src/Core/ResourceManager.h](src/Core/ResourceManager.h))
- Loads spritesheets from YAML manifests
- Provides centralized sprite/animation access
- Manages resource lifetime

## YAML Manifest Format

### Basic Structure

```yaml
spritesheet:
  name: "unique_id"
  texture: "path/to/texture.png"

  sprites:
    - # Sprite definitions here
```

### Sprite Types

#### 1. Single Sprite

Static, non-animated sprite:

```yaml
- name: "tree"
  type: "single"
  x: 0
  y: 0
  width: 32
  height: 48
```

#### 2. Animation

Sequential frames in a grid layout:

```yaml
- name: "walk_right"
  type: "animation"
  x: 0                # Starting X position
  y: 0                # Starting Y position
  frame_width: 48     # Width of each frame
  frame_height: 48    # Height of each frame
  frame_count: 6      # Total number of frames
  columns: 6          # Frames per row (wraps to next row)
  frame_duration: 0.1 # Seconds per frame
  loop: true          # Loop the animation
```

**Frame Layout:**
Frames are read left-to-right, top-to-bottom:
```
[0][1][2][3][4][5]
[6][7][8]...
```

#### 3. Grid-based Tileset

Auto-generates sprites from a uniform grid:

```yaml
- name: "terrain"
  type: "grid"
  tile_width: 16
  tile_height: 16
  columns: 0  # Auto-calculate from texture width
  rows: 0     # Auto-calculate from texture height
```

Generates sprites named: `terrain_0`, `terrain_1`, `terrain_2`, etc.

#### 4. Manual Frames

For irregular animations with non-uniform frame sizes:

```yaml
- name: "special_effect"
  type: "frames"
  loop: false
  frame_list:
    - { x: 0, y: 0, width: 32, height: 32, duration: 0.05 }
    - { x: 32, y: 0, width: 48, height: 48, duration: 0.1 }
    - { x: 80, y: 0, width: 64, height: 64, duration: 0.15 }
```

## Usage Examples

### Loading Resources

```cpp
#include "Core/ResourceManager.h"

// In your Application class
class GameApp : public Runa::Application {
    std::unique_ptr<Runa::ResourceManager> m_resources;

    void onInit() override {
        // Create resource manager
        m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());

        // Load sprite manifests
        m_resources->loadSpriteSheetFromYAML("Resources/manifests/player.yaml");
        m_resources->loadSpriteSheetFromYAML("Resources/manifests/slime.yaml");
        m_resources->loadSpriteSheetFromYAML("Resources/manifests/tilesets.yaml");
    }
};
```

### Accessing Sprites

```cpp
// Method 1: Get spritesheet, then sprite
auto* playerSheet = m_resources->getSpriteSheet("player");
if (playerSheet) {
    auto* walkAnim = playerSheet->getSprite("walk_down");
    if (walkAnim) {
        // walkAnim contains frame data
    }
}

// Method 2: Direct access
auto* idleAnim = m_resources->getSprite("player", "idle_right");
if (idleAnim) {
    // Use animation
}
```

### Rendering Sprites (Conceptual)

```cpp
void drawSprite(const Sprite* sprite, int frameIndex, int x, int y) {
    if (!sprite || sprite->frames.empty()) return;

    // Get current frame
    const SpriteFrame& frame = sprite->getFrame(frameIndex);

    // Get texture
    const Texture& texture = spriteSheet->getTexture();

    // Render using SDL3 GPU
    // (This requires setting up a sprite rendering pipeline)
    // renderTextureRegion(texture, frame.x, frame.y, frame.width, frame.height, x, y);
}
```

### Animation Controller Example

```cpp
class AnimationController {
    const Sprite* m_currentAnim = nullptr;
    size_t m_currentFrame = 0;
    float m_frameTimer = 0.0f;

    void setAnimation(const Sprite* anim) {
        if (m_currentAnim != anim) {
            m_currentAnim = anim;
            m_currentFrame = 0;
            m_frameTimer = 0.0f;
        }
    }

    void update(float deltaTime) {
        if (!m_currentAnim || m_currentAnim->frames.empty()) return;

        m_frameTimer += deltaTime;

        const SpriteFrame& frame = m_currentAnim->getFrame(m_currentFrame);

        if (m_frameTimer >= frame.duration) {
            m_frameTimer = 0.0f;
            m_currentFrame++;

            if (m_currentFrame >= m_currentAnim->getFrameCount()) {
                if (m_currentAnim->loop) {
                    m_currentFrame = 0;  // Loop
                } else {
                    m_currentFrame = m_currentAnim->getFrameCount() - 1;  // Stay on last frame
                }
            }
        }
    }

    const SpriteFrame& getCurrentFrame() const {
        return m_currentAnim->getFrame(m_currentFrame);
    }
};
```

## Included Manifests

### Player ([Resources/manifests/player.yaml](Resources/manifests/player.yaml))

**Animations:**
- `idle_down`, `idle_up`, `idle_right`, `idle_left` (6 frames, 48x48)
- `walk_down`, `walk_up`, `walk_right`, `walk_left` (6 frames, 48x48)
- `attack_down`, `attack_up` (4 frames, 48x48, non-looping)

**Texture:** 288x480 pixels (6x10 grid of 48x48 frames)

### Slime ([Resources/manifests/slime.yaml](Resources/manifests/slime.yaml))

**Animations:**
- `idle` (6 frames, 32x48)
- `walk` (6 frames, 32x48)
- `attack` (6 frames, 32x48, non-looping)

**Texture:** 192x144 pixels (6x3 grid of 32x48 frames)

### Plains Tileset ([Resources/manifests/tilesets.yaml](Resources/manifests/tilesets.yaml))

**Tiles:**
- Auto-generated grid of 16x16 tiles
- Named `plains_tile_0`, `plains_tile_1`, etc.

## Creating Custom Manifests

### Workflow

1. **Prepare your spritesheet image**
   - Use PNG format with transparency
   - Organize frames in a grid layout
   - Note frame dimensions and positions

2. **Create YAML manifest**
   ```bash
   touch Resources/manifests/my_sprite.yaml
   ```

3. **Define the spritesheet**
   ```yaml
   spritesheet:
     name: "my_sprite"
     texture: "../path/to/my_sprite.png"
     sprites:
       # Add sprite definitions
   ```

4. **Load in your game**
   ```cpp
   m_resources->loadSpriteSheetFromYAML("Resources/manifests/my_sprite.yaml");
   ```

### Tips

- **Measure precisely** - Use image editing software (GIMP, Photoshop) to get exact pixel positions
- **Power-of-two textures** - While not strictly required, 256x256, 512x512, etc. are optimal
- **Consistent frame sizes** - Makes animation definitions simpler
- **Descriptive names** - Use `character_walk_north` instead of `anim1`
- **Group related sprites** - One manifest per character/tileset
- **Set loop appropriately** - `false` for one-shot animations (attacks, explosions)

## Performance Considerations

### Texture Atlasing
- **Combine sprites** - Pack multiple related sprites into one texture
- **Reduce draw calls** - One texture = one GPU texture switch
- **GPU memory** - Large atlases use more VRAM but reduce overhead

### Best Practices
- Load resources during initialization, not during gameplay
- Reuse SpriteSheet references instead of looking them up every frame
- Cache frequently used sprite pointers
- Use grid-based tilesets for terrain to minimize metadata

## Troubleshooting

### Manifest not loading
```
Error: Failed to load texture: ../path/to/sprite.png
```
**Solution:** Check that texture path is relative to the YAML file, not the executable.

### Sprite not found
```cpp
auto* sprite = sheet->getSprite("walk_down");
// sprite is nullptr
```
**Solution:** Verify sprite name matches YAML definition exactly (case-sensitive).

### Animation not looping
**Solution:** Check `loop: true` in the YAML animation definition.

### Incorrect frame positions
**Solution:** Verify `x`, `y`, `frame_width`, `frame_height` values match your sprite sheet layout.

## Future Enhancements

Potential additions to the system:

- **9-slice sprites** - For scalable UI elements
- **Pivot points** - Custom sprite origins
- **Sprite batching** - Combine multiple sprite draws into one GPU call
- **Texture compression** - Reduce memory usage
- **Hot reloading** - Reload manifests at runtime
- **Atlas packing** - Auto-generate texture atlases from individual images
- **Animation events** - Trigger callbacks at specific frames

## See Also

- [ARCHITECTURE.md](ARCHITECTURE.md) - Overall engine architecture
- [STRUCTURE.md](STRUCTURE.md) - Project organization
- [Resources/manifests/README.md](Resources/manifests/README.md) - Manifest format reference
