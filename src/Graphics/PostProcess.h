// File: src/Graphics/PostProcess.h

#ifndef RUNA_GRAPHICS_POSTPROCESS_H
#define RUNA_GRAPHICS_POSTPROCESS_H

#include "Renderer.h"
#include "RunaAPI.h"
#include <SDL3/SDL.h>

namespace Runa {



class RUNA_API PostProcess {
public:
  explicit PostProcess(Renderer &renderer);
  ~PostProcess() = default;


  void applyPsychedelic(float /*time*/) {

  }

private:
  Renderer &m_renderer;
};

}

#endif
