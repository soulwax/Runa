// File: src/Graphics/PostProcess.cpp
// DEPRECATED: Vulkan2D uses internal shaders and rendering pipeline
// This is a stub for API compatibility

#include "../runapch.h"
#include "PostProcess.h"
#include "Core/Log.h"

namespace Runa {

PostProcess::PostProcess(Renderer& renderer)
    : m_renderer(renderer) {
    LOG_INFO("PostProcess created (stub - post-processing not implemented with Vulkan2D)");
}

// Empty stub implementation - all functionality would need custom Vulkan2D implementation

} // namespace Runa
