// File: src/Graphics/ImGui.cpp

#include "ImGui.h"
#include "../runapch.h"
#include "Core/Log.h"
#include <VK2D/VK2D.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

// Forward declare Vulkan2D functions we need
extern "C" {
    VkInstance vk2dVulkanGetInstance();
    VkDevice vk2dVulkanGetDevice();
    VkPhysicalDevice vk2dVulkanGetPhysicalDevice();
    VkQueue vk2dVulkanGetQueue();
    uint32_t vk2dVulkanGetQueueFamily();
    uint32_t vk2dVulkanGetSwapchainImageCount();
    uint32_t vk2dVulkanGetMaxFramesInFlight();
}

namespace Runa {

ImGuiManager::ImGuiManager(Window& window, Renderer& renderer)
    : m_window(window), m_renderer(renderer) {
    initialize();
}

ImGuiManager::~ImGuiManager() {
    shutdown();
}

void ImGuiManager::initialize() {
    if (m_initialized) {
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // Docking support - only enable if available (requires docking branch or newer version)
    // For now, we'll skip docking to ensure compatibility
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    // Note: SDL3 backend is disabled due to API compatibility issues
    // We'll handle SDL3 input manually in processEvent()
    // ImGui_ImplSDL3_InitForVulkan(m_window.getHandle());  // Disabled

    // Note: Full Vulkan backend integration requires:
    // 1. VkInstance, VkDevice, VkQueue (available via vk2dVulkanGet* functions)
    // 2. VkRenderPass (Vulkan2D manages this internally - needs integration)
    // 3. Descriptor pool creation
    // 4. Proper synchronization with Vulkan2D's rendering pipeline
    //
    // For now, SDL3 backend is initialized for input handling.
    // Vulkan rendering integration will be added in a future update.

    LOG_INFO("ImGui initialized (SDL3 input backend ready)");
    m_initialized = true;
}

void ImGuiManager::shutdown() {
    if (!m_initialized) {
        return;
    }

    // ImGui_ImplSDL3_Shutdown();  // Disabled - SDL3 backend not used
    // ImGui_ImplVulkan_Shutdown(); // Will be called when we have proper cleanup

    if (m_context) {
        ImGui::DestroyContext(m_context);
        m_context = nullptr;
    }

    m_initialized = false;
    LOG_INFO("ImGui shutdown");
}

void ImGuiManager::processEvent(const SDL_Event& event) {
    if (!m_initialized) {
        return;
    }
    // Manual SDL3 event processing for ImGui
    // Since ImGui_ImplSDL3 is disabled, we'll handle this manually
    ImGuiIO& io = ImGui::GetIO();

    switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION:
            io.AddMousePosEvent((float)event.motion.x, (float)event.motion.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int button = -1;
            if (event.button.button == SDL_BUTTON_LEFT) button = 0;
            else if (event.button.button == SDL_BUTTON_RIGHT) button = 1;
            else if (event.button.button == SDL_BUTTON_MIDDLE) button = 2;
            if (button >= 0) {
                io.AddMouseButtonEvent(button, (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN));
            }
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
            io.AddMouseWheelEvent((float)event.wheel.x, (float)event.wheel.y);
            break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            // Basic key handling - full implementation would map all SDL3 keys
            io.AddKeyEvent(ImGuiKey_ModCtrl, (SDL_GetModState() & SDL_KMOD_CTRL) != 0);
            io.AddKeyEvent(ImGuiKey_ModShift, (SDL_GetModState() & SDL_KMOD_SHIFT) != 0);
            io.AddKeyEvent(ImGuiKey_ModAlt, (SDL_GetModState() & SDL_KMOD_ALT) != 0);
            io.AddKeyEvent(ImGuiKey_ModSuper, (SDL_GetModState() & SDL_KMOD_GUI) != 0);
            break;
        }
        case SDL_EVENT_TEXT_INPUT:
            io.AddInputCharactersUTF8(event.text.text);
            break;
    }
}

void ImGuiManager::beginFrame() {
    if (!m_initialized) {
        return;
    }

    // Manual frame setup since SDL3 backend is disabled
    ImGuiIO& io = ImGui::GetIO();

    // Update display size (SDL3 returns bool)
    int width = 0, height = 0;
    if (SDL_GetWindowSize(m_window.getHandle(), &width, &height)) {
        io.DisplaySize = ImVec2((float)width, (float)height);
    } else {
        // Fallback to stored window size
        io.DisplaySize = ImVec2((float)m_window.getWidth(), (float)m_window.getHeight());
    }

    // Update delta time
    // SDL3: SDL_GetTicks() returns Uint64 (milliseconds)
    static Uint64 lastTime = 0;
    Uint64 currentTime = SDL_GetTicks();
    if (lastTime == 0) {
        lastTime = currentTime;
    }
    io.DeltaTime = ((float)(currentTime - lastTime)) / 1000.0f;
    if (io.DeltaTime <= 0.0f || io.DeltaTime > 1.0f) {
        io.DeltaTime = 1.0f / 60.0f;  // Fallback to 60 FPS if invalid
    }
    lastTime = currentTime;

    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    if (!m_initialized) {
        return;
    }

    ImGui::Render();

    // Note: ImGui_ImplVulkan_RenderDrawData() needs to be called
    // This requires access to the command buffer and render pass
    // We'll need to integrate this with Vulkan2D's rendering pipeline

    // For now, we'll render ImGui after the main rendering
    // This will be handled in the Renderer or Application class
}

bool ImGuiManager::wantsCaptureMouse() const {
    if (!m_initialized) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool ImGuiManager::wantsCaptureKeyboard() const {
    if (!m_initialized) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

} // namespace Runa
