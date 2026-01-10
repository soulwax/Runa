
#ifndef RUNA_GRAPHICS_PIXEL_SCALE_H
#define RUNA_GRAPHICS_PIXEL_SCALE_H

#include "../RunaAPI.h"

namespace Runa {

/**
 * PixelScale - Centralized pixel scaling system for the engine
 * 
 * This utility provides helper functions to work with the global pixel scale,
 * ensuring consistent scaling throughout the engine. All sprites are rendered
 * at 3x their logical size by default (each pixel becomes 3x3).
 */
class RUNA_API PixelScale {
public:
    /**
     * Get the current global pixel scale factor
     * @return The pixel scale (default: 3.0f)
     */
    static float getScale() { return s_scale; }
    
    /**
     * Set the global pixel scale factor
     * @param scale The new pixel scale (should be > 0)
     */
    static void setScale(float scale) {
        if (scale > 0.0f) {
            s_scale = scale;
        }
    }
    
    /**
     * Convert a logical size to scaled size
     * @param logicalSize Size in logical pixels (e.g., 16 for a tile)
     * @return Size in scaled pixels (e.g., 48 for scale=3.0)
     */
    static float toScaled(float logicalSize) {
        return logicalSize * s_scale;
    }
    
    /**
     * Convert a scaled size to logical size
     * @param scaledSize Size in scaled pixels
     * @return Size in logical pixels
     */
    static float toLogical(float scaledSize) {
        return scaledSize / s_scale;
    }
    
    /**
     * Get the scaled tile size for a given logical tile size
     * @param logicalTileSize Logical tile size (typically 16)
     * @return Scaled tile size (e.g., 48 for scale=3.0)
     */
    static int getScaledTileSize(int logicalTileSize = 16) {
        return static_cast<int>(logicalTileSize * s_scale);
    }
    
    /**
     * Calculate how many tiles fit in a given logical distance
     * @param logicalDistance Distance in logical pixels
     * @param logicalTileSize Logical tile size (typically 16)
     * @return Number of tiles that fit (in logical space, one per tile)
     */
    static int getScaledTileCount(int logicalDistance, int logicalTileSize = 16) {
        return logicalDistance / logicalTileSize;
    }
    
    /**
     * Calculate position for scaled tiles (e.g., for fences)
     * Places tiles at scaled intervals to prevent overlap
     * @param startPos Starting position in logical pixels
     * @param tileIndex Index of the tile (0, 1, 2, ...)
     * @param logicalTileSize Logical tile size (typically 16)
     * @return Position in logical pixels where the tile should be placed
     */
    static int getScaledTilePosition(int startPos, int tileIndex, int logicalTileSize = 16) {
        return startPos + tileIndex * getScaledTileSize(logicalTileSize);
    }

private:
    static float s_scale;
};

} // namespace Runa

#endif // RUNA_GRAPHICS_PIXEL_SCALE_H
