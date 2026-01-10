

#include "Application.h"
#include "../runapch.h"
#include "Log.h"
#include "SceneManager.h"

namespace Runa {

Application::Application(const std::string &title, int width, int height) {





  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    throw std::runtime_error(std::string("Failed to initialize SDL: ") +
                             SDL_GetError());
  }

  LOG_INFO("SDL3 initialized successfully");



  if (!TTF_Init()) {
    LOG_WARN("Failed to initialize SDL_ttf: {}", SDL_GetError());

  } else {
    LOG_INFO("SDL3_ttf initialized successfully");
  }


  m_window = std::make_unique<Window>(title, width, height);
  m_renderer = std::make_unique<Renderer>(*m_window);


  m_input = std::make_unique<Input>(*m_window);
  m_window->setInput(m_input.get());
  LOG_INFO("Input system initialized");


  m_sceneManager = std::make_unique<SceneManager>();
  LOG_INFO("SceneManager initialized");
}

Application::~Application() {
  onShutdown();

  m_renderer.reset();
  m_window.reset();


  if (TTF_WasInit()) {
    TTF_Quit();
  }

  SDL_Quit();
  LOG_INFO("Application shut down");
  Log::shutdown();
}

SceneManager &Application::getSceneManager() {
  return *m_sceneManager;
}

void Application::run() {
  m_running = true;


  try {
    onInit();
    LOG_INFO("onInit() completed successfully");
  } catch (const std::exception &e) {
    LOG_CRITICAL("Exception in onInit(): {}", e.what());
    throw;
  } catch (...) {
    LOG_CRITICAL("Unknown exception in onInit()");
    throw;
  }

  LOG_INFO("Starting main loop...");
  mainLoop();
}

void Application::mainLoop() {
  using Clock = std::chrono::high_resolution_clock;
  using Duration = std::chrono::duration<float>;
  using namespace std::chrono_literals;

  auto lastTime = Clock::now();
  float accumulatedTime = 0.0f;
  int frameCount = 0;

  while (m_running && !m_window->shouldClose()) {


    auto currentTime = Clock::now();
    float deltaTime = Duration(currentTime - lastTime).count();
    lastTime = currentTime;


    accumulatedTime += deltaTime;
    frameCount++;
    if (accumulatedTime >= 1.0f) {
      m_currentFPS = frameCount;


      LOG_DEBUG("FPS: {}", m_currentFPS);
      frameCount = 0;
      accumulatedTime = 0.0f;
    }


    m_window->processEvents();


    m_input->beginFrame();


    if (m_sceneManager->hasScenes()) {
      m_sceneManager->update(deltaTime);
    } else {
      onUpdate(deltaTime);
    }


    m_renderer->beginFrame();
    if (m_sceneManager->hasScenes()) {
      m_sceneManager->render();
    } else {
      onRender();
    }
    m_renderer->endFrame();
  }
}

}
