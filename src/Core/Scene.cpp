// File: src/Core/Scene.cpp

/**
 * Scene.cpp
 * Implementation of the Scene base class.
 */

#include "runapch.h"
#include "Scene.h"
#include "Application.h"
#include "../Graphics/Renderer.h"
#include "Input.h"

namespace Runa {

	Scene::Scene(Application& app, const std::string& name)
		: m_app(app), m_name(name), m_type("Scene"), m_transparent(false) {
	}

	Renderer& Scene::getRenderer() {
		return m_app.getRenderer();
	}

	Input& Scene::getInput() {
		return m_app.getInput();
	}

}
