

#include "ImGui.h"
#include "../runapch.h"
#include "Core/Log.h"
#include <VK2D/VK2D.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>


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


    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;





    ImGui::StyleColorsDark();












    //



    LOG_INFO("ImGui initialized (SDL3 input backend ready)");
    m_initialized = true;
}

void ImGuiManager::shutdown() {
    if (!m_initialized) {
        return;
    }




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


    ImGuiIO& io = ImGui::GetIO();


    int width = 0, height = 0;
    if (SDL_GetWindowSize(m_window.getHandle(), &width, &height)) {
        io.DisplaySize = ImVec2((float)width, (float)height);
    } else {

        io.DisplaySize = ImVec2((float)m_window.getWidth(), (float)m_window.getHeight());
    }



    static Uint64 lastTime = 0;
    Uint64 currentTime = SDL_GetTicks();
    if (lastTime == 0) {
        lastTime = currentTime;
    }
    io.DeltaTime = ((float)(currentTime - lastTime)) / 1000.0f;
    if (io.DeltaTime <= 0.0f || io.DeltaTime > 1.0f) {
        io.DeltaTime = 1.0f / 60.0f;
    }
    lastTime = currentTime;

    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    if (!m_initialized) {
        return;
    }

    ImGui::Render();







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

}
