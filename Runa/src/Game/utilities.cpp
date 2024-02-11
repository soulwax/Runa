#include "runapch.h"
#include "utilities.h"

SDL_Surface* LoadBmp(char const* path)
{
	SDL_Surface* imageSurface = SDL_LoadBMP(path);

	if (!imageSurface)
		return nullptr;

	return imageSurface;
}