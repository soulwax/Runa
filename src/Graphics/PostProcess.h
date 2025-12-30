// File: src/Graphics/PostProcess.h

#ifndef RUNA_GRAPHICS_POSTPROCESS_H
#define RUNA_GRAPHICS_POSTPROCESS_H

#include "Renderer.h"
#include "RunaAPI.h"
#include "Shader.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <memory>

namespace Runa {

class RUNA_API PostProcess {
public:
  explicit PostProcess(Renderer &renderer);
  ~PostProcess() = default;

  void applyPsychedelic(SDL_GPUCommandBuffer *cmdBuffer,
                        SDL_GPUTexture *sourceTexture,
                        SDL_GPUTexture *targetTexture, float time);

private:
  Renderer &m_renderer;
  std::shared_ptr<Shader> m_psychedelicShader;

  void initializeShader();
  void renderFullScreenQuad(SDL_GPUCommandBuffer *cmdBuffer,
                            SDL_GPUTexture *sourceTexture,
                            SDL_GPUTexture *targetTexture, float time);
};

} // namespace Runa

#endif // RUNA_GRAPHICS_POSTPROCESS_H
