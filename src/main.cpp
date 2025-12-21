// File: src/main.cpp

#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Runa2 - SDL3 Test",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL3 initialized successfully!" << std::endl;
    std::cout << "Window created. Press Ctrl+C to exit or close the window." << std::endl;

    // Main loop
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Simple delay to prevent CPU spinning
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "SDL3 cleanup complete." << std::endl;
    return 0;
}
