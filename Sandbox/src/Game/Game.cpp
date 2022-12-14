#include "runapch.h"
#include "Game.h"
#include <iostream>

SDL_Renderer* Game::Renderer = nullptr;

Game::Game()
{
	//nullptr init
	Window = nullptr;
	IsRunning = true;
}
Game::~Game() = default;

void Game::Init(const char* title, int windowWidth, int windowHeight, bool isFullscreen)
{
	this->Init(title, 0, 0, windowWidth, windowHeight, isFullscreen, false);
}

void Game::Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen, bool vsync)
{
	int window_flags = 0;
	int renderer_flags = SDL_RENDERER_ACCELERATED;
	if (fullscreen) window_flags |= SDL_WINDOW_FULLSCREEN;
	if (vsync) renderer_flags |= SDL_RENDERER_PRESENTVSYNC;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
	{
		std::cout << "Subsystems Initialized!..." << std::endl;

		Window = SDL_CreateWindow(title, xpos, ypos, width, height, window_flags);
		if (Window)
		{
			std::cout << "Window created!" << std::endl;
		}

		// RENDERER CREATION
		Game::Renderer = SDL_CreateRenderer(Window, -1, renderer_flags);
		if (Game::Renderer)
		{
			std::cout << "Renderer created!" << std::endl;
		}

		IsRunning = true;
	}
	else
	{
		IsRunning = false;
	}

	//TODO: Init Assets, Audio, Controllers, Game State (Menu, Level X etc.) etc.
}

void Game::HandleEventsGlobally()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		IsRunning = false;
		break;
		//TODO: Integrate Input Handler here
	case SDL_KEYUP:
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			IsRunning = false;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

auto Game::Update() -> void
{
}

void Game::Render()
{
	SDL_RenderClear(Game::Renderer);

	// Main render code here

	SDL_RenderPresent(Game::Renderer);
}

void Game::Clean() const
{
	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(Game::Renderer);
	SDL_Quit();

	std::cout << "Game Cleaned" << std::endl;
}

bool Game::IsGameRunning()
{
	return IsRunning;
}
