// File: src/Core/Application.h

#ifndef RUNA_CORE_APPLICATION_H
#define RUNA_CORE_APPLICATION_H

#include "../Graphics/Window.h"
#include "../RunaAPI.h"
#include "Graphics/Renderer.h"
#include "Input.h"
#include <memory>

namespace Runa {


class SceneManager;

class RUNA_API Application {
public:
  Application(const std::string &title, int width, int height);
  virtual ~Application();

  void run();


  Window &getWindow() { return *m_window; }
  Renderer &getRenderer() { return *m_renderer; }
  Input &getInput() { return *m_input; }
  SceneManager &getSceneManager();


  int getFPS() const { return m_currentFPS; }

protected:
  virtual void onInit() {}
  virtual void onUpdate(float deltaTime) {}
  virtual void onRender() {}
  virtual void onShutdown() {}

private:
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Input> m_input;
  std::unique_ptr<SceneManager> m_sceneManager;
  bool m_running = false;
  int m_currentFPS = 0;

  void mainLoop();
};

}
#endif
