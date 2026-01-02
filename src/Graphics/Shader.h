// File: src/Graphics/Shader.h
// DEPRECATED: Vulkan2D uses internal shaders
// This is a stub for API compatibility

#ifndef RUNA_GRAPHICS_SHADER_H
#define RUNA_GRAPHICS_SHADER_H

#include "RunaAPI.h"

namespace Runa {

// Stub Shader class - Vulkan2D uses internal shaders
// Kept for API compatibility with existing code
class RUNA_API Shader {
public:
  Shader() = default;
  ~Shader() = default;

  // Disable copying and moving
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;
  Shader(Shader &&) = delete;
  Shader &operator=(Shader &&) = delete;

  // Always returns false - shaders are handled internally by Vulkan2D
  bool isValid() const { return false; }
};

} // namespace Runa

#endif // RUNA_GRAPHICS_SHADER_H
