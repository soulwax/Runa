#include "CollisionMask.h"
#include "../Core/Log.h"
#include <algorithm>

namespace Runa {

CollisionMask::CollisionMask(int width, int height)
    : m_width(width), m_height(height) {
    if (width > 0 && height > 0) {
        // Calculate bytes needed (8 pixels per byte, rounded up)
        size_t totalBits = static_cast<size_t>(width) * height;
        size_t bytes = (totalBits + 7) >> 3;  // Divide by 8, round up
        m_data.resize(bytes, 0);
    }
}

CollisionMask CollisionMask::fromAlphaChannel(const uint8_t* pixels, int width, int height,
                                               int stride, uint8_t alphaThreshold) {
    CollisionMask mask(width, height);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // RGBA format: alpha is at offset 3
            const uint8_t* pixel = pixels + (y * stride) + (x * 4);
            uint8_t alpha = pixel[3];
            mask.setPixel(x, y, alpha >= alphaThreshold);
        }
    }
    
    return mask;
}

CollisionMask CollisionMask::solid(int width, int height) {
    CollisionMask mask(width, height);
    // Fill all bits with 1
    std::fill(mask.m_data.begin(), mask.m_data.end(), 0xFF);
    return mask;
}

CollisionMask CollisionMask::empty(int width, int height) {
    // Default constructor already initializes to 0
    return CollisionMask(width, height);
}

bool CollisionMask::isPixelSolid(int x, int y) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return false;
    }
    
    size_t bitIndex = getBitIndex(x, y);
    size_t byteIndex = bitIndex >> 3;  // Divide by 8
    size_t bitOffset = bitIndex & 7;   // Mod 8
    
    return (m_data[byteIndex] & (1 << bitOffset)) != 0;
}

void CollisionMask::setPixel(int x, int y, bool solid) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }
    
    size_t bitIndex = getBitIndex(x, y);
    size_t byteIndex = bitIndex >> 3;
    size_t bitOffset = bitIndex & 7;
    
    if (solid) {
        m_data[byteIndex] |= (1 << bitOffset);
    } else {
        m_data[byteIndex] &= ~(1 << bitOffset);
    }
}

bool CollisionMask::collidesWith(const CollisionMask& other, int offsetX, int offsetY) const {
    // Calculate overlap region
    int startX = std::max(0, offsetX);
    int startY = std::max(0, offsetY);
    int endX = std::min(m_width, offsetX + other.m_width);
    int endY = std::min(m_height, offsetY + other.m_height);
    
    // Check each pixel in the overlap region
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (isPixelSolid(x, y) && other.isPixelSolid(x - offsetX, y - offsetY)) {
                return true;
            }
        }
    }
    
    return false;
}

bool CollisionMask::collidesWithPoint(int x, int y) const {
    return isPixelSolid(x, y);
}

bool CollisionMask::collidesWithAABB(int aabbX, int aabbY, int aabbWidth, int aabbHeight) const {
    // Calculate overlap region with this mask
    int startX = std::max(0, aabbX);
    int startY = std::max(0, aabbY);
    int endX = std::min(m_width, aabbX + aabbWidth);
    int endY = std::min(m_height, aabbY + aabbHeight);
    
    // Check each pixel in the overlap region
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (isPixelSolid(x, y)) {
                return true;
            }
        }
    }
    
    return false;
}

size_t CollisionMask::getBitIndex(int x, int y) const {
    return static_cast<size_t>(y) * m_width + x;
}

} // namespace Runa
