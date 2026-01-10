// File: src/Scenes/MenuScene.h

/**
 * MenuScene.h
 * Main menu scene - demonstrates scene transitions.
 */

#ifndef RUNA_MENUSCENE_H
#define RUNA_MENUSCENE_H

#include "../Core/Scene.h"
#include "../Graphics/Font.h"
#include "../Graphics/SpriteBatch.h"
#include <memory>

namespace Runa {

class RUNA_API MenuScene : public Scene {
public:
  MenuScene(Application &app);
  ~MenuScene() override = default;

  void onEnter() override;
  void onExit() override;
  void onUpdate(float dt) override;
  void onRender() override;

private:
  std::unique_ptr<SpriteBatch> m_spriteBatch;
  std::unique_ptr<Font> m_font;
  std::unique_ptr<Texture> m_titleTexture;
  std::unique_ptr<Texture> m_startTexture;

  float m_blinkTimer = 0.0f;
  bool m_showStart = true;
};

} // namespace Runa

#endif // RUNA_MENUSCENE_H
