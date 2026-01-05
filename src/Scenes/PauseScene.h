/**
 * PauseScene.h
 * Pause menu overlay scene - demonstrates scene stacking.
 */

#pragma once

#include "../Core/Scene.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/Font.h"
#include <memory>

namespace Runa {

	class RUNA_API PauseScene : public Scene {
	public:
		PauseScene(Application& app);
		~PauseScene() override = default;

		void onEnter() override;
		void onExit() override;
		void onUpdate(float dt) override;
		void onRender() override;

	private:
		std::unique_ptr<SpriteBatch> m_spriteBatch;
		std::unique_ptr<Font> m_font;
		std::unique_ptr<Texture> m_pausedTexture;
		std::unique_ptr<Texture> m_resumeTexture;
		std::unique_ptr<Texture> m_quitTexture;
	};

}  // namespace Runa
