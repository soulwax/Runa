#include "runapch.h"
#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(const char* path, int row, int column) {
    // Load the sprite sheet as a surface
    SDL_Surface* tempSurface = LoadBmp(path);
    if (!tempSurface) {
        // Handle error
        RUNA_ERROR("Failed to load sprite sheet: {}", path);
        return;
    }

    // Convert the surface to a texture
    SpritesheetTexture = SDL_CreateTextureFromSurface(Game::Renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Set sprite dimensions
    spriteWidth = tempSurface->w / column;
    spriteHeight = tempSurface->h / row;

    // Initialize the clip rectangle
    Clip.x = 0;
    Clip.y = 0;
    Clip.w = spriteWidth;
    Clip.h = spriteHeight;
}

SpriteSheet::~SpriteSheet() {
    if (SpritesheetTexture) {
        SDL_DestroyTexture(SpritesheetTexture);
    }
}

void SpriteSheet::SelectSprite(int x, int y) {
    Clip.x = x * spriteWidth;
    Clip.y = y * spriteHeight;
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position) {
    if (!SpritesheetTexture) {
        RUNA_ERROR("Spritesheet texture is null");
        return;
    }

    // Render the selected sprite
    SDL_RenderCopy(renderer, SpritesheetTexture, &Clip, position);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    if (!SpritesheetTexture) {
        RUNA_ERROR("Spritesheet texture is null");
        return;
    }

    // Render the selected sprite with additional transformations
    SDL_RenderCopyEx(renderer, SpritesheetTexture, &Clip, position, angle, center, flip);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Rect* clip) {
    if (!SpritesheetTexture) {
        RUNA_ERROR("Spritesheet texture is null");
        return;
    }
    SDL_RenderCopyEx(renderer, SpritesheetTexture, clip, position, angle, center, flip);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, SDL_Rect* clip) {
    DrawSelectedSprite(renderer, position, clip, 0.0, nullptr, SDL_FLIP_NONE);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    DrawSelectedSprite(renderer, position, clip, angle, center, flip, {255, 255, 255, 255}, 255);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Color color) {
    DrawSelectedSprite(renderer, position, clip, angle, center, flip, color, 255);
}

void SpriteSheet::DrawSelectedSprite(SDL_Renderer* renderer, SDL_Rect* position, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Color color, int alpha) {
    if (!SpritesheetTexture) {
        RUNA_ERROR("Spritesheet texture is null");
        return;
    }
    SDL_SetTextureColorMod(SpritesheetTexture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(SpritesheetTexture, alpha);
    SDL_RenderCopyEx(renderer, SpritesheetTexture, clip, position, angle, center, flip);
}

