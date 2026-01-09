/**
 * SimpleShapes.cpp
 * Implementation of simple shape rendering utilities
 */

#include "../runapch.h"
#include "SimpleShapes.h"
#include "../Core/Log.h"

namespace Runa {

SimpleShapes::SimpleShapes(Renderer& renderer) {

    unsigned char whitePixel[4] = {255, 255, 255, 255};

    m_whitePixel = std::make_unique<Texture>(renderer, whitePixel, 1, 1);

    if (m_whitePixel && m_whitePixel->isValid()) {
        LOG_DEBUG("SimpleShapes: Created white pixel texture");
    } else {
        LOG_ERROR("SimpleShapes: Failed to create white pixel texture");
    }
}

}
