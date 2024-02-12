#pragma once
#include "runapch.h"
#include <SDL.h>
#include "utilities.h"
#include "Game.h"

class SpriteSheet
{
public:
	SpriteSheet(const char *path, int row, int column);
	~SpriteSheet();

	void SelectSprite(int x, int y);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, double angle, SDL_Point *center, SDL_RendererFlip flip);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, double angle, SDL_Point *center, SDL_RendererFlip flip, SDL_Rect *clip);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, SDL_Rect *clip);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip, SDL_Color color);
	void DrawSelectedSprite(SDL_Renderer *renderer, SDL_Rect *position, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip, SDL_Color color, int alpha);

private:
	SDL_Texture *SpritesheetTexture;
	SDL_Rect Clip;
	int spriteWidth;
	int spriteHeight;
};
