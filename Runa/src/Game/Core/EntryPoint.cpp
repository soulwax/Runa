// RuneBurst.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "runapch.h"
#include "Game/Core/Base.h"
#include "Game/Game.h"

//Screen constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 640;
constexpr int MIN_SCREEN_FPS = 60;
constexpr int MAX_SCREEN_FPS = 240;

//screen init variables (with default values)
bool isFullscreen = false;
bool isVsync = true;
int screenFps = 60;
unsigned int screenTicksPerFrame = 1000 / screenFps;

//debugging
bool verbose = true;

void ProcessArgs(const int argc, const char* argv[])
{
	const std::vector<std::string> arguments(argv, argv + argc);
	for (const std::string& s : arguments) {
		if (s == "-verbose=true") verbose = true;
		else if (s == "-verbose=false") verbose = false;
		if (s == "-vsync=true") isVsync = true;
		else if (s == "-vsync=false") isVsync = false;
		if (s == "-fullscreen=true") isFullscreen = true;
		else if (s == "-fullscreen=false") isFullscreen = false;
		//...

		if (verbose) RUNA_INFO("Argument: {0}", s);
	}
	if (verbose) RUNA_INFO("Finished processing arguments. Amount: " + std::to_string(arguments.size()));
}

int main(const int argc, const char* argv[])
{
	Runa::Log::Init();
	//look at args first and discern whether they make sense
	ProcessArgs(argc, argv);

	//process screen vars
	if (isVsync) screenFps = MAX_SCREEN_FPS;
	else screenFps = MIN_SCREEN_FPS;
	screenTicksPerFrame = 1000 / screenFps;

	//init engine backend
	const auto game = std::make_unique<Game>();
	game->Init("Runa", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, isFullscreen, isVsync);

	int ticks = 0;
	unsigned int lastFrameTime = SDL_GetTicks();

	//BEGIN GAME LOOP
	while (game->IsGameRunning())
	{
		const Uint32 now = SDL_GetTicks();

		//### MAIN GAME LOOP COMMENCE ###
		game->HandleEventsGlobally();
		Game::Update();
		Game::Render();
		//### GAME LOOP END ###

		//tick counter
		ticks++;

		const unsigned int frameTime = SDL_GetTicks() - now;

		//this makes my eyes bleed but it is
		//needed for now to make the GPU go less APESHIT when
		//no VSYNC is activated
		//tl;dr simple frame limiter using MIN_SCREEN_FPS for reference
		if (!isVsync && screenTicksPerFrame > frameTime)
		{
			SDL_Delay(screenTicksPerFrame - frameTime);
		}

		//Reset counted ticks every second and display in console
		//At the moment this is a horrible idea as this is equivalent
		//to frames per second.
		if (SDL_GetTicks() - lastFrameTime >= 1000)
		{
			RUNA_INFO("FPS: {0}", ticks);
			lastFrameTime += 1000;
			ticks = 0;
		}
	}

	//keeping it explicit, could integrate into destructor
	game->Clean();
	return 0;
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg:
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
