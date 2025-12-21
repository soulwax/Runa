// File: src/Graphics/Texture.cpp

#include "Texture.h"
#include "Renderer.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <stdexcept>

namespace Runa {

Texture::Texture(Renderer& renderer, const std::string& path)
    : m_device(renderer.getDevice()) {
    if (!m_device) {
        throw std::runtime_error("Invalid GPU device for texture creation");
    }
    loadFromFile(path);
}

Texture::Texture(Renderer& renderer, int width, int height, const void* pixelData)
    : m_device(renderer.getDevice()) {
    if (!m_device) {
        throw std::runtime_error("Invalid GPU device for texture creation");
    }
    createFromPixels(width, height, pixelData);
}

Texture::~Texture() {
    if (m_texture) {
        SDL_ReleaseGPUTexture(m_device, m_texture);
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_device(other.m_device),
      m_texture(other.m_texture),
      m_width(other.m_width),
      m_height(other.m_height) {
    other.m_device = nullptr;
    other.m_texture = nullptr;
    other.m_width = 0;
    other.m_height = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_texture) {
            SDL_ReleaseGPUTexture(m_device, m_texture);
        }

        m_device = other.m_device;
        m_texture = other.m_texture;
        m_width = other.m_width;
        m_height = other.m_height;

        other.m_device = nullptr;
        other.m_texture = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }
    return *this;
}

void Texture::loadFromFile(const std::string& path) {
    // Load image using SDL_image
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        throw std::runtime_error("Failed to load texture: " + path + " - " + SDL_GetError());
    }

    m_width = surface->w;
    m_height = surface->h;

    // Convert surface to RGBA8888 format if needed
    SDL_Surface* rgbaSurface = surface;
    if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);
        if (!rgbaSurface) {
            throw std::runtime_error("Failed to convert surface format for: " + path);
        }
    }

    // Create GPU texture
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width = m_width;
    textureInfo.height = m_height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

    m_texture = SDL_CreateGPUTexture(m_device, &textureInfo);
    if (!m_texture) {
        SDL_DestroySurface(rgbaSurface);
        throw std::runtime_error("Failed to create GPU texture for: " + path + " - " + SDL_GetError());
    }

    // Upload texture data
    SDL_GPUTextureTransferInfo transferInfo{};
    transferInfo.transfer_buffer = nullptr;  // We'll use a direct upload

    SDL_GPUTextureRegion textureRegion{};
    textureRegion.texture = m_texture;
    textureRegion.w = m_width;
    textureRegion.h = m_height;
    textureRegion.d = 1;

    // Create command buffer for upload
    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(m_device);
    if (!uploadCmd) {
        SDL_DestroySurface(rgbaSurface);
        throw std::runtime_error("Failed to acquire command buffer for texture upload");
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);
    if (copyPass) {
        SDL_GPUTextureTransferInfo transferSrc{};
        transferSrc.pixels = rgbaSurface->pixels;
        transferSrc.pixels_per_row = m_width;
        transferSrc.rows_per_layer = m_height;

        SDL_UploadToGPUTexture(copyPass, &transferSrc, &textureRegion, false);
        SDL_EndGPUCopyPass(copyPass);
    }

    SDL_SubmitGPUCommandBuffer(uploadCmd);

    SDL_DestroySurface(rgbaSurface);
    std::cout << "Loaded texture: " << path << " (" << m_width << "x" << m_height << ")" << std::endl;
}

void Texture::createFromPixels(int width, int height, const void* pixelData) {
    m_width = width;
    m_height = height;

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    textureInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

    m_texture = SDL_CreateGPUTexture(m_device, &textureInfo);
    if (!m_texture) {
        throw std::runtime_error(std::string("Failed to create GPU texture from pixels: ") + SDL_GetError());
    }

    if (pixelData) {
        // Upload pixel data
        SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(m_device);
        if (uploadCmd) {
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);
            if (copyPass) {
                SDL_GPUTextureTransferInfo transferSrc{};
                transferSrc.pixels = const_cast<void*>(pixelData);
                transferSrc.pixels_per_row = width;
                transferSrc.rows_per_layer = height;

                SDL_GPUTextureRegion textureRegion{};
                textureRegion.texture = m_texture;
                textureRegion.w = width;
                textureRegion.h = height;
                textureRegion.d = 1;

                SDL_UploadToGPUTexture(copyPass, &transferSrc, &textureRegion, false);
                SDL_EndGPUCopyPass(copyPass);
            }
            SDL_SubmitGPUCommandBuffer(uploadCmd);
        }
    }

    std::cout << "Created texture from pixels (" << width << "x" << height << ")" << std::endl;
}

} // namespace Runa
