// File: src/Scenes/PauseScene.cpp

/**
 * PauseScene.cpp
 * Implementation of the pause menu overlay scene.
 */

#include "../runapch.h"
#include "PauseScene.h"
#include "../Core/Application.h"
#include "../Core/Log.h"
#include "../Core/SceneManager.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Window.h"
#include "MenuScene.h"

namespace Runa {

PauseScene::PauseScene(Application &app) : Scene(app, "PauseScene") {
  setTransparent(true); // Render game scene behind
}

void PauseScene::onEnter() {
  LOG_INFO("PauseScene: Entering");

  m_spriteBatch = std::make_unique<SpriteBatch>(getRenderer());
  m_font =
      std::make_unique<Font>(getRenderer(), "Resources/Fonts/Renogare.ttf", 48);

  SDL_Color white = {255, 255, 255, 255};
  SDL_Color yellow = {255, 255, 0, 255};
  SDL_Color red = {255, 100, 100, 255};

  m_pausedTexture = m_font->renderText("PAUSED", white);
  m_resumeTexture = m_font->renderText("Press ESC to Resume", yellow);
  m_quitTexture = m_font->renderText("Press Q to Quit", red);
}

void PauseScene::onExit() { LOG_INFO("PauseScene: Exiting"); }

void PauseScene::onUpdate(float dt) {
  // Resume game
  if (getInput().isKeyPressed(SDLK_ESCAPE)) {
    LOG_INFO("Resuming game...");
    getApp().getSceneManager().popScene();
  }

  // Quit to menu
  if (getInput().isKeyPressed(SDLK_Q)) {
    LOG_INFO("Quitting to menu...");
    getApp().getSceneManager().changeScene(
        std::make_unique<MenuScene>(getApp()));
  }
}

void PauseScene::onRender() {
  // Semi-transparent overlay
  getRenderer().clear(0.0f, 0.0f, 0.0f, 0.5f);

  m_spriteBatch->begin();

  int winWidth = getApp().getWindow().getWidth();
  int winHeight = getApp().getWindow().getHeight();

  // Draw "PAUSED" title
  if (m_pausedTexture) {
    int x = (winWidth - static_cast<int>(m_pausedTexture->getWidth())) >> 1;
    int y = winHeight / 3;
    m_spriteBatch->draw(*m_pausedTexture, x, y);
  }

  // Draw "Resume" instruction
  if (m_resumeTexture) {
    int x = (winWidth - static_cast<int>(m_resumeTexture->getWidth())) >> 1;
    int y = winHeight >> 1;
    m_spriteBatch->draw(*m_resumeTexture, x, y);
  }

  // Draw "Quit" instruction
  if (m_quitTexture) {
    int x = (winWidth - static_cast<int>(m_quitTexture->getWidth())) >> 1;
    int y = (winHeight >> 1) + 60;
    m_spriteBatch->draw(*m_quitTexture, x, y);
  }

  m_spriteBatch->end();
}

} // namespace Runa
