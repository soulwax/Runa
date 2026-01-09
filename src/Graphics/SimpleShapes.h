/**
 * SimpleShapes.h
 * Utility for rendering simple colored shapes for prototyping
 */

#ifndef RUNA_GRAPHICS_SIMPLESHAPES_H
#define RUNA_GRAPHICS_SIMPLESHAPES_H

#include "../RunaAPI.h"
#include "Texture.h"
#include "Renderer.h"
#include <memory>

namespace Runa {

class RUNA_API SimpleShapes {
public:
    SimpleShapes(Renderer& renderer);
    ~SimpleShapes() = default;


    Texture* getWhitePixel() { return m_whitePixel.get(); }

private:
    std::unique_ptr<Texture> m_whitePixel;
};

}

#endif
