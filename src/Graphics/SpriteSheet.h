// File: src/Graphics/SpriteSheet.h

#ifndef RUNA_GRAPHICS_SPRITESHEET_H
#define RUNA_GRAPHICS_SPRITESHEET_H

#include "RunaAPI.h"
#include "Texture.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Runa
{

    // Represents a single frame in a sprite animation
    struct RUNA_API SpriteFrame
    {
        int x = 0;             // X position in texture
        int y = 0;             // Y position in texture
        int width = 0;         // Frame width
        int height = 0;        // Frame height
        float duration = 0.1f; // Duration for animation (seconds)
    };

    // Represents a sprite animation or single sprite
    struct RUNA_API Sprite
    {
        std::string name;
        std::vector<SpriteFrame> frames;
        bool loop = true;

        // Get frame at specific index
        const SpriteFrame &getFrame(size_t index) const
        {
            return frames[index % frames.size()];
        }

        size_t getFrameCount() const { return frames.size(); }
        bool isAnimation() const { return frames.size() > 1; }
    };

    // Manages a spritesheet texture and its sprite definitions
    class RUNA_API SpriteSheet
    {
    public:
        SpriteSheet(Renderer &renderer, const std::string &texturePath);
        ~SpriteSheet() = default;

        // Disable copying
        SpriteSheet(const SpriteSheet &) = delete;
        SpriteSheet &operator=(const SpriteSheet &) = delete;

        // Enable moving
        SpriteSheet(SpriteSheet &&) noexcept = default;
        SpriteSheet &operator=(SpriteSheet &&) noexcept = default;

        // Add a single sprite frame
        void addSprite(const std::string &name, int x, int y, int width, int height);

        // Add an animation with multiple frames
        void addAnimation(const std::string &name, int x, int y, int frameWidth, int frameHeight,
                          int frameCount, int columns, float frameDuration = 0.1f, bool loop = true);

        // Add frames manually
        void addSpriteFrames(const std::string &name, const std::vector<SpriteFrame> &frames, bool loop = true);

        // Get sprite by name
        const Sprite *getSprite(const std::string &name) const;
        const Texture &getTexture() const { return *m_texture; }

        // Get all sprite names
        std::vector<std::string> getSpriteNames() const;

        // Utility: Create a grid-based spritesheet (for tilemaps)
        void createGrid(const std::string &baseName, int tileWidth, int tileHeight, int columns = 0, int rows = 0);

    private:
        std::unique_ptr<Texture> m_texture;
        std::unordered_map<std::string, Sprite> m_sprites;
    };

} // namespace Runa

#endif // RUNA_GRAPHICS_SPRITESHEET_H
