# Spritesheet Quick Start Guide

Get up and running with the Runa2 spritesheet system in minutes.

## Quick Example

```cpp
#include "Core/Application.h"
#include "Core/ResourceManager.h"

class MyGame : public Runa::Application {
    std::unique_ptr<Runa::ResourceManager> m_resources;

public:
    MyGame() : Application("My Game", 1280, 720) {}

protected:
    void onInit() override {
        // Create resource manager
        m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());

        // Load player sprites
        m_resources->loadSpriteSheetFromYAML("Resources/manifests/player.yaml");

        // Get a sprite
        auto* walkAnim = m_resources->getSprite("player", "walk_down");
        if (walkAnim) {
            std::cout << "Loaded animation with "
                      << walkAnim->getFrameCount() << " frames" << std::endl;
        }
    }
};
```

## Three-Step Setup

### 1. Create Your Sprite YAML

`Resources/manifests/my_character.yaml`:

```yaml
spritesheet:
  name: "my_character"
  texture: "../mystic_woods_2.2/sprites/characters/player.png"

  sprites:
    - name: "idle"
      type: "animation"
      x: 0
      y: 0
      frame_width: 48
      frame_height: 48
      frame_count: 6
      columns: 6
      frame_duration: 0.15
      loop: true
```

### 2. Load in Your Game

```cpp
void onInit() override {
    m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());
    m_resources->loadSpriteSheetFromYAML("Resources/manifests/my_character.yaml");
}
```

### 3. Use the Sprites

```cpp
auto* sprite = m_resources->getSprite("my_character", "idle");
const SpriteFrame& frame = sprite->getFrame(0);  // Get first frame

std::cout << "Frame position: " << frame.x << ", " << frame.y << std::endl;
std::cout << "Frame size: " << frame.width << "x" << frame.height << std::endl;
```

## Common Patterns

### Loading Multiple Spritesheets

```cpp
void onInit() override {
    m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());

    // Load all your assets
    m_resources->loadSpriteSheetFromYAML("Resources/manifests/player.yaml");
    m_resources->loadSpriteSheetFromYAML("Resources/manifests/enemies.yaml");
    m_resources->loadSpriteSheetFromYAML("Resources/manifests/environment.yaml");
}
```

### Creating a Tileset

For a 16x16 tile grid:

```yaml
spritesheet:
  name: "terrain"
  texture: "../path/to/tileset.png"

  sprites:
    - name: "tile"
      type: "grid"
      tile_width: 16
      tile_height: 16
```

Access tiles as: `tile_0`, `tile_1`, `tile_2`, etc.

### Static Sprite

For non-animated sprites:

```yaml
- name: "tree"
  type: "single"
  x: 0
  y: 0
  width: 32
  height: 64
```

### Non-Looping Animation

For one-shot animations (attacks, explosions):

```yaml
- name: "explosion"
  type: "animation"
  x: 0
  y: 0
  frame_width: 64
  frame_height: 64
  frame_count: 8
  columns: 4
  frame_duration: 0.05
  loop: false  # Animation plays once and stops
```

## Included Examples

The engine comes with ready-to-use manifests for the Mystic Woods assets:

### Player Character

```cpp
m_resources->loadSpriteSheetFromYAML("Resources/manifests/player.yaml");

auto* idle = m_resources->getSprite("player", "idle_down");
auto* walk = m_resources->getSprite("player", "walk_right");
auto* attack = m_resources->getSprite("player", "attack_down");
```

**Available animations:**
- Idle: `idle_down`, `idle_up`, `idle_right`, `idle_left`
- Walk: `walk_down`, `walk_up`, `walk_right`, `walk_left`
- Attack: `attack_down`, `attack_up`

### Slime Enemy

```cpp
m_resources->loadSpriteSheetFromYAML("Resources/manifests/slime.yaml");

auto* slimeIdle = m_resources->getSprite("slime", "idle");
auto* slimeWalk = m_resources->getSprite("slime", "walk");
auto* slimeAttack = m_resources->getSprite("slime", "attack");
```

### Terrain Tiles

```cpp
m_resources->loadSpriteSheetFromYAML("Resources/manifests/tilesets.yaml");

auto* tile0 = m_resources->getSprite("plains_tileset", "plains_tile_0");
auto* tile1 = m_resources->getSprite("plains_tileset", "plains_tile_1");
```

## Animation Playback Example

```cpp
class Character {
    const Runa::Sprite* m_currentAnimation = nullptr;
    size_t m_currentFrame = 0;
    float m_timer = 0.0f;

public:
    void setAnimation(const Runa::Sprite* anim) {
        if (m_currentAnimation != anim) {
            m_currentAnimation = anim;
            m_currentFrame = 0;
            m_timer = 0.0f;
        }
    }

    void update(float deltaTime) {
        if (!m_currentAnimation) return;

        m_timer += deltaTime;
        const auto& frame = m_currentAnimation->getFrame(m_currentFrame);

        if (m_timer >= frame.duration) {
            m_timer = 0.0f;
            m_currentFrame++;

            if (m_currentFrame >= m_currentAnimation->getFrameCount()) {
                if (m_currentAnimation->loop) {
                    m_currentFrame = 0;
                } else {
                    m_currentFrame = m_currentAnimation->getFrameCount() - 1;
                }
            }
        }
    }

    const Runa::SpriteFrame& getCurrentFrame() const {
        return m_currentAnimation->getFrame(m_currentFrame);
    }
};
```

Usage:

```cpp
Character player;
auto* walkAnim = m_resources->getSprite("player", "walk_right");
player.setAnimation(walkAnim);

// In update loop
player.update(deltaTime);
const auto& frame = player.getCurrentFrame();
// Render frame at player position
```

## Tips

1. **Path is relative to YAML file**
   ```yaml
   texture: "../mystic_woods_2.2/sprites/characters/player.png"
   #        ^ Relative to Resources/manifests/
   ```

2. **Case-sensitive names**
   ```cpp
   getSprite("player", "walk_down");  // ✓ Correct
   getSprite("player", "Walk_Down");  // ✗ Won't find it
   ```

3. **Check for nullptr**
   ```cpp
   auto* sprite = m_resources->getSprite("player", "idle");
   if (sprite) {
       // Use sprite
   } else {
       std::cerr << "Sprite not found!" << std::endl;
   }
   ```

4. **Frame indexing is safe**
   ```cpp
   sprite->getFrame(1000);  // Safe - wraps around using modulo
   ```

## Next Steps

- See [SPRITESHEET_SYSTEM.md](SPRITESHEET_SYSTEM.md) for complete documentation
- See [Resources/manifests/README.md](Resources/manifests/README.md) for YAML format reference
- Check example manifests in `Resources/manifests/`

## Common Issues

**"Failed to load texture"**
- Check that the texture path is correct relative to the YAML file
- Verify the PNG file exists

**"Sprite not found"**
- Verify sprite name matches YAML exactly (case-sensitive)
- Check that the manifest was loaded successfully

**Animation not playing**
- Ensure you're calling `update()` with deltaTime
- Verify `frame_duration` isn't too long
- Check that you're rendering the current frame
