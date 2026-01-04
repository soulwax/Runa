// File: src/Graphics/PostProcess.h
// DEPRECATED: Vulkan2D uses internal shaders and rendering pipeline
// This is a stub for API compatibility

#ifndef RUNA_GRAPHICS_POSTPROCESS_H
#define RUNA_GRAPHICS_POSTPROCESS_H

#include "Renderer.h"
#include "RunaAPI.h"
#include <SDL3/SDL.h>

namespace Runa {

// Stub PostProcess class - Vulkan2D post-processing would require custom implementation
// Kept for API compatibility with existing code
class RUNA_API PostProcess {
public:
  explicit PostProcess(Renderer &renderer);
  ~PostProcess() = default;

  // Stub method - does nothing with Vulkan2D
  void applyPsychedelic(float /*time*/) {
    // No-op - post-processing not implemented with Vulkan2D
  }

private:
  Renderer &m_renderer;
};

} // namespace Runa

#endif // RUNA_GRAPHICS_POSTPROCESS_H
