



#include "../runapch.h"
#include "PostProcess.h"
#include "Core/Log.h"

namespace Runa {

PostProcess::PostProcess(Renderer& renderer)
    : m_renderer(renderer) {
    LOG_INFO("PostProcess created (stub - post-processing not implemented with Vulkan2D)");
}



}
