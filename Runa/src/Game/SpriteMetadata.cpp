#include "runapch.h"
#include "SpriteMetadata.h"

void SpriteMetadata::AddSprite(const std::string &name, const SDL_Rect &rect)
{
	metadata[name] = rect;
}

bool SpriteMetadata::GetSprite(const std::string &name, SDL_Rect &rect) const
{
	auto it = metadata.find(name);
	if (it != metadata.end())
	{
		rect = it->second;
		return true;
	}
	return false;
}

void SpriteMetadata::SaveToFile(const std::string &filePath) const
{
	nlohmann::json j;
	for (const auto &pair : metadata)
	{
		j[pair.first] = {{"x", pair.second.x}, {"y", pair.second.y}, {"w", pair.second.w}, {"h", pair.second.h}};
	}
	std::ofstream file(filePath);
	file << j.dump(4);
}

void SpriteMetadata::LoadFromFile(const std::string &filePath)
{
	std::ifstream file(filePath);
	nlohmann::json j;
	file >> j;
	for (auto &element : j.items())
	{
		SDL_Rect rect = {
			element.value()["x"],
			element.value()["y"],
			element.value()["w"],
			element.value()["h"]};
		metadata[element.key()] = rect;
	}
}
