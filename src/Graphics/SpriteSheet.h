

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


    struct RUNA_API SpriteFrame
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        float duration = 0.1f;
    };


    struct RUNA_API Sprite
    {
        std::string name;
        std::vector<SpriteFrame> frames;
        bool loop = true;


        const SpriteFrame &getFrame(size_t index) const
        {
            return frames[index % frames.size()];
        }

        size_t getFrameCount() const { return frames.size(); }
        bool isAnimation() const { return frames.size() > 1; }
    };


    class RUNA_API SpriteSheet
    {
    public:
        SpriteSheet(Renderer &renderer, const std::string &texturePath);
        ~SpriteSheet() = default;


        SpriteSheet(const SpriteSheet &) = delete;
        SpriteSheet &operator=(const SpriteSheet &) = delete;


        SpriteSheet(SpriteSheet &&) noexcept = default;
        SpriteSheet &operator=(SpriteSheet &&) noexcept = default;


        void addSprite(const std::string &name, int x, int y, int width, int height);


        void addAnimation(const std::string &name, int x, int y, int frameWidth, int frameHeight,
                          int frameCount, int columns, float frameDuration = 0.1f, bool loop = true);


        void addSpriteFrames(const std::string &name, const std::vector<SpriteFrame> &frames, bool loop = true);


        const Sprite *getSprite(const std::string &name) const;
        const Texture &getTexture() const { return *m_texture; }


        std::vector<std::string> getSpriteNames() const;


        void createGrid(const std::string &baseName, int tileWidth, int tileHeight, int columns = 0, int rows = 0);

    private:
        std::unique_ptr<Texture> m_texture;
        std::unordered_map<std::string, Sprite> m_sprites;
    };

}

#endif
