/**
 * MenuScene.cpp
 * Implementation of the main menu scene.
 */

#include "MenuScene.h"
#include "../Core/Application.h"
#include "../Core/Log.h"
#include "../Core/SceneManager.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Window.h"
#include "../runapch.h"
#include "GameScene.h"

namespace Runa {

MenuScene::MenuScene(Application &app) : Scene(app, "MenuScene") {}

void MenuScene::onEnter() {
  LOG_INFO("MenuScene: Entering");

  m_spriteBatch = std::make_unique<SpriteBatch>(getRenderer());
  m_font =
      std::make_unique<Font>(getRenderer(), "Resources/Fonts/Renogare.ttf", 48);


  SDL_Color white = {255, 255, 255, 255};
  SDL_Color yellow = {255, 255, 0, 255};
  m_titleTexture = m_font->renderText("RUNA2 ENGINE", white);
  m_startTexture = m_font->renderText("Press SPACE to Start", yellow);
}

void MenuScene::onExit() { LOG_INFO("MenuScene: Exiting"); }

void MenuScene::onUpdate(float dt) {

  m_blinkTimer += dt;
  if (m_blinkTimer >= 0.5f) {
    m_showStart = !m_showStart;
    m_blinkTimer = 0.0f;
  }


  if (getInput().isKeyPressed(SDLK_SPACE)) {
    LOG_INFO("Starting game...");
    getApp().getSceneManager().changeScene(
        std::make_unique<GameScene>(getApp()));
  }
}

void MenuScene::onRender() {
  getRenderer().clear(0.05f, 0.05f, 0.1f, 1.0f);

  m_spriteBatch->begin();


  int winWidth = getApp().getWindow().getWidth();
  int winHeight = getApp().getWindow().getHeight();


  if (m_titleTexture) {
    int titleX = (winWidth - static_cast<int>(m_titleTexture->getWidth())) / 2;
    int titleY = winHeight / 3;
    m_spriteBatch->draw(*m_titleTexture, titleX, titleY);
  }


  if (m_startTexture && m_showStart) {
    int startX = (winWidth - static_cast<int>(m_startTexture->getWidth())) / 2;
    int startY = winHeight / 2;
    m_spriteBatch->draw(*m_startTexture, startX, startY);
  }

  m_spriteBatch->end();
}

}
