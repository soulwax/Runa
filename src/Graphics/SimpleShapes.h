/**
 * SimpleShapes.h
 * Utility for rendering simple colored shapes for prototyping
 */

#pragma once

#include "../RunaAPI.h"
#include "Texture.h"
#include "Renderer.h"
#include <memory>

namespace Runa {

class RUNA_API SimpleShapes {
public:
    SimpleShapes(Renderer& renderer);
    ~SimpleShapes() = default;

    // Get the white pixel texture (used for tinting)
    Texture* getWhitePixel() { return m_whitePixel.get(); }

private:
    std::unique_ptr<Texture> m_whitePixel;
};

} // namespace Runa
