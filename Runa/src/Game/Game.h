#pragma once
#include "SDL.h"

#undef main

class Game
{
public:
	Game();
	~Game();

	void Init(const char* title, int windowWidth, int windowHeight, bool isFullscreen);
	void Init(const char* title, int xPos, int yPos, int windowWidth, int windowHeight, bool isFullscreen, bool isVsync);

	void HandleEventsGlobally();
	static void Update();
	static void Render();
	void Clean() const;

	bool IsGameRunning();

	static constexpr int RENDER_SCALE = 2; // global pixel scaling
	static SDL_Renderer* Renderer;

private:
	bool IsRunning = false;
	SDL_Window* Window;
};
