#pragma once
#include <SDL.h>
#include <json/json.hpp>

class SpriteMetadata
{
public:
	void AddSprite(const std::string &name, const SDL_Rect &rect);
	bool GetSprite(const std::string &name, SDL_Rect &rect) const;
	void SaveToFile(const std::string &filePath) const;
	void LoadFromFile(const std::string &filePath);

protected:
	std::unordered_map<std::string, SDL_Rect> metadata;
};
// Path: Runa/src/Game/SpriteMetadata.cpp
