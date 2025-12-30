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
    SDL_zero(shaderInfo);

    shaderInfo.code = spirvCode.data();
    shaderInfo.code_size = spirvCode.size();
    shaderInfo.stage = stage;
    shaderInfo.entrypoint = "main";
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;

    // Fragment shaders with texture sampling need sampler count specified
    // Vertex shaders with push constants need uniform buffer count specified
    if (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) {
        shaderInfo.num_samplers = 1;  // One texture sampler at binding 0
        shaderInfo.num_uniform_buffers = 0;
        shaderInfo.num_storage_buffers = 0;
        shaderInfo.num_storage_textures = 0;
    } else if (stage == SDL_GPU_SHADERSTAGE_VERTEX) {
        shaderInfo.num_samplers = 0;
        shaderInfo.num_uniform_buffers = 0;  // No uniforms for fixed shader
        shaderInfo.num_storage_buffers = 0;
        shaderInfo.num_storage_textures = 0;
    }

    SDL_GPUShader* shader = SDL_CreateGPUShader(m_device, &shaderInfo);
    if (!shader) {
        LOG_ERROR("Failed to create GPU shader: {}", SDL_GetError());
    }

    return shader;
}

} // namespace Runa
