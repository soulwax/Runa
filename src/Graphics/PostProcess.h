// File: src/Graphics/PostProcess.h

#pragma once

#include "../RunaAPI.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <memory>

namespace Runa {

class RUNA_API PostProcess {
public:
    explicit PostProcess(Renderer& renderer);
    ~PostProcess() = default;

    // Apply psychedelic effect to the screen
    void applyPsychedelic(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUTexture* sourceTexture, SDL_GPUTexture* targetTexture, float time);

private:
    Renderer& m_renderer;
    std::shared_ptr<Shader> m_psychedelicShader;
    
    void initializeShader();
    void renderFullScreenQuad(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUTexture* sourceTexture, SDL_GPUTexture* targetTexture, float time);
};

} // namespace Runa

