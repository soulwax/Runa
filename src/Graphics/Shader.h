// File: src/Shader.h

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace Runa {

class Shader {
public:
    Shader(SDL_GPUDevice* device, const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Disable copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Enable moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    SDL_GPUShader* getVertexShader() const { return m_vertexShader; }
    SDL_GPUShader* getFragmentShader() const { return m_fragmentShader; }

    bool isValid() const { return m_vertexShader != nullptr && m_fragmentShader != nullptr; }

private:
    SDL_GPUDevice* m_device = nullptr;
    SDL_GPUShader* m_vertexShader = nullptr;
    SDL_GPUShader* m_fragmentShader = nullptr;

    std::vector<uint8_t> readFile(const std::string& path);
    SDL_GPUShader* createShaderFromSPIRV(const std::vector<uint8_t>& spirvCode, SDL_GPUShaderStage stage);
};

} // namespace Runa
