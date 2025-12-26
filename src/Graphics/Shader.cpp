// File: src/Graphics/Shader.cpp

#include "../runapch.h"
#include "Shader.h"
#include "Core/Log.h"

namespace Runa {

Shader::Shader(SDL_GPUDevice* device, const std::string& vertexPath, const std::string& fragmentPath)
    : m_device(device) {

    if (!m_device) {
        throw std::runtime_error("Invalid GPU device for shader creation");
    }

    // Read SPIR-V files
    auto vertexCode = readFile(vertexPath);
    auto fragmentCode = readFile(fragmentPath);

    // Create shaders
    m_vertexShader = createShaderFromSPIRV(vertexCode, SDL_GPU_SHADERSTAGE_VERTEX);
    m_fragmentShader = createShaderFromSPIRV(fragmentCode, SDL_GPU_SHADERSTAGE_FRAGMENT);

    if (!isValid()) {
        throw std::runtime_error("Failed to create shaders from: " + vertexPath + ", " + fragmentPath);
    }

    LOG_INFO("Shaders created successfully from {} and {}", vertexPath, fragmentPath);
}

Shader::~Shader() {
    if (m_vertexShader) {
        SDL_ReleaseGPUShader(m_device, m_vertexShader);
    }
    if (m_fragmentShader) {
        SDL_ReleaseGPUShader(m_device, m_fragmentShader);
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_device(other.m_device),
      m_vertexShader(other.m_vertexShader),
      m_fragmentShader(other.m_fragmentShader) {
    other.m_device = nullptr;
    other.m_vertexShader = nullptr;
    other.m_fragmentShader = nullptr;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        if (m_vertexShader) {
            SDL_ReleaseGPUShader(m_device, m_vertexShader);
        }
        if (m_fragmentShader) {
            SDL_ReleaseGPUShader(m_device, m_fragmentShader);
        }

        // Transfer ownership
        m_device = other.m_device;
        m_vertexShader = other.m_vertexShader;
        m_fragmentShader = other.m_fragmentShader;

        other.m_device = nullptr;
        other.m_vertexShader = nullptr;
        other.m_fragmentShader = nullptr;
    }
    return *this;
}

std::vector<uint8_t> Shader::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    LOG_DEBUG("Read shader file: {} ({} bytes)", path, fileSize);
    return buffer;
}

SDL_GPUShader* Shader::createShaderFromSPIRV(const std::vector<uint8_t>& spirvCode, SDL_GPUShaderStage stage) {
    SDL_GPUShaderCreateInfo shaderInfo{};
    shaderInfo.code = spirvCode.data();
    shaderInfo.code_size = spirvCode.size();
    shaderInfo.stage = stage;
    shaderInfo.entrypoint = "main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;

    SDL_GPUShader* shader = SDL_CreateGPUShader(m_device, &shaderInfo);
    if (!shader) {
        LOG_ERROR("Failed to create GPU shader: {}", SDL_GetError());
    }

    return shader;
}

} // namespace Runa
