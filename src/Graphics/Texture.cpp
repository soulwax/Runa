// File: src/Graphics/Texture.cpp

#include "../runapch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Core/Log.h"

namespace Runa {

Texture::Texture(Renderer& renderer, const std::string& path) {
    if (!renderer.isValid()) {
        throw std::runtime_error("Invalid Vulkan2D renderer for texture creation");
    }
    loadFromFile(path);
}

Texture::Texture(Renderer& renderer, int width, int height, const void* pixelData) {
    if (!renderer.isValid()) {
        throw std::runtime_error("Invalid Vulkan2D renderer for texture creation");
    }
    createFromPixels(width, height, pixelData);
}

Texture::~Texture() {
    if (m_texture) {
        vk2dTextureFree(m_texture);
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_texture(other.m_texture),
      m_width(other.m_width),
      m_height(other.m_height) {
    other.m_texture = nullptr;
    other.m_width = 0;
    other.m_height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_texture) {
            vk2dTextureFree(m_texture);
        }

        m_texture = other.m_texture;
        m_width = other.m_width;
        m_height = other.m_height;

        other.m_texture = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void Texture::loadFromFile(const std::string& path) {
    // Vulkan2D has built-in texture loading from files (PNG, BMP, JPG, TIFF)
    m_texture = vk2dTextureLoad(path.c_str());

    if (!m_texture) {
        throw std::runtime_error("Failed to load texture: " + path);
    }

    // Get dimensions from loaded texture
    m_width = static_cast<int>(vk2dTextureWidth(m_texture));
    m_height = static_cast<int>(vk2dTextureHeight(m_texture));

    LOG_INFO("Texture loaded: {} ({}x{})", path, m_width, m_height);
}

void Texture::createFromPixels(int width, int height, const void* pixelData) {
    m_width = width;
    m_height = height;

    if (pixelData) {
        // Vulkan2D: Create image from RGBA pixel data
        // vk2dImageFromPixels expects RGBA pixels (4 bytes per pixel)
        VK2DLogicalDevice device = vk2dRendererGetDevice();
        VK2DImage image = vk2dImageFromPixels(device, pixelData, width, height, true);

        if (!image) {
            throw std::runtime_error("Failed to create image from pixels");
        }

        // Create texture from image
        m_texture = vk2dTextureLoadFromImage(image);

        // Free the intermediate image (texture now owns the data)
        vk2dImageFree(image);

        if (!m_texture) {
            throw std::runtime_error("Failed to create texture from image");
        }
    } else {
        // Create blank texture
        m_texture = vk2dTextureCreate(static_cast<float>(width), static_cast<float>(height));

        if (!m_texture) {
            throw std::runtime_error("Failed to create blank texture");
        }
    }

    LOG_DEBUG("Created texture from pixels ({}x{})", width, height);
}

} // namespace Runa
