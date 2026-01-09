

#ifndef RUNA_GRAPHICS_IMGUI_H
#define RUNA_GRAPHICS_IMGUI_H

#include "../RunaAPI.h"
#include "Window.h"
#include "Renderer.h"
#include <imgui.h>

#include <imgui_impl_vulkan.h>
#include <memory>

namespace Runa {

/**
 * @brief ImGui wrapper for Runa2 engine
 *
 * Provides a C++ interface to Dear ImGui for creating immediate-mode GUIs.
 * Handles initialization, event processing, and rendering integration with Vulkan2D.
 */
class RUNA_API ImGuiManager {
public:
    ImGuiManager(Window& window, Renderer& renderer);
    ~ImGuiManager();


    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;


    ImGuiManager(ImGuiManager&&) noexcept = delete;
    ImGuiManager& operator=(ImGuiManager&&) noexcept = delete;

    /**
     * @brief Process SDL event for ImGui
     * @param event SDL event to process
     */
    void processEvent(const SDL_Event& event);

    /**
     * @brief Begin a new ImGui frame
     * Call this at the start of each frame, before any ImGui calls
     */
    void beginFrame();

    /**
     * @brief End ImGui frame and render
     * Call this at the end of each frame, after all ImGui calls
     */
    void endFrame();

    /**
     * @brief Get the ImGui context
     * @return Pointer to ImGui context
     */
    ImGuiContext* getContext() const { return m_context; }

    /**
     * @brief Check if ImGui wants to capture mouse/keyboard input
     * @return True if ImGui is using input, false otherwise
     */
    bool wantsCaptureMouse() const;
    bool wantsCaptureKeyboard() const;

private:
    Window& m_window;
    Renderer& m_renderer;
    ImGuiContext* m_context = nullptr;
    bool m_initialized = false;

    void initialize();
    void shutdown();
};

}

#endif
