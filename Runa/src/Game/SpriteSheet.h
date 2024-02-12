#pragma once
#include "runapch.h"
#include <SDL.h>
#include "utilities.h"

/**
 * \brief SpriteSheet class
 * path: path to the sprite sheet
 * row: number of rows in the sprite sheet
 * column: number of columns in the sprite sheet
 */
class SpriteSheet
{
public:
	SpriteSheet(char const* path, int row, int column);
	~SpriteSheet();

	void SelectSprite(int x, int y);
	void DrawSelectedSprite(SDL_Surface* windowSurface, SDL_Rect* position);

private:
	SDL_Rect Clip;
	SDL_Surface* SpritesheetImage;
};
