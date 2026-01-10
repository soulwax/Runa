// File: src/Graphics/Shader.h

#ifndef RUNA_GRAPHICS_SHADER_H
#define RUNA_GRAPHICS_SHADER_H

#include "RunaAPI.h"

namespace Runa {



class RUNA_API Shader {
public:
  Shader() = default;
  ~Shader() = default;


  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;
  Shader(Shader &&) = delete;
  Shader &operator=(Shader &&) = delete;


  bool isValid() const { return false; }
};

}

#endif
