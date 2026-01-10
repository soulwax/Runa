#ifndef RUNA_COLLISION_COLLISIONMASK_H
#define RUNA_COLLISION_COLLISIONMASK_H

#include "../RunaAPI.h"
#include <vector>
#include <cstdint>

namespace Runa {

/**
 * CollisionMask stores a pixel-perfect collision bitmap for a sprite/tile.
 * Each bit represents whether that pixel is solid (1) or passable (0).
 */
class RUNA_API CollisionMask {
public:
    CollisionMask() = default;
    CollisionMask(int width, int height);
    
    // Create mask from alpha channel of pixel data (RGBA format)
    // alphaThreshold: pixels with alpha >= threshold are solid
    static CollisionMask fromAlphaChannel(const uint8_t* pixels, int width, int height, 
                                          int stride, uint8_t alphaThreshold = 128);
    
    // Create a full solid rectangle mask
    static CollisionMask solid(int width, int height);
    
    // Create an empty (no collision) mask
    static CollisionMask empty(int width, int height);
    
    // Check if a pixel is solid
    bool isPixelSolid(int x, int y) const;
    
    // Set a pixel's collision state
    void setPixel(int x, int y, bool solid);
    
    // Check if this mask collides with another mask at given offsets
    // offsetX/Y: position of 'other' relative to 'this'
    bool collidesWith(const CollisionMask& other, int offsetX, int offsetY) const;
    
    // Check if a point collides with this mask
    bool collidesWithPoint(int x, int y) const;
    
    // Check if an AABB collides with this mask
    // Returns true if ANY solid pixel in the mask is within the AABB
    bool collidesWithAABB(int aabbX, int aabbY, int aabbWidth, int aabbHeight) const;
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isValid() const { return m_width > 0 && m_height > 0; }
    
    // Get the underlying bit data (for serialization/debugging)
    const std::vector<uint8_t>& getData() const { return m_data; }

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<uint8_t> m_data;  // Packed bits, 8 pixels per byte
    
    // Helper to get bit index
    size_t getBitIndex(int x, int y) const;
};

} // namespace Runa

#endif // RUNA_COLLISION_COLLISIONMASK_H
