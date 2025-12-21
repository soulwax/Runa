// File: src/Graphics/Texture.cpp

#include "Texture.h"
#include "Renderer.h"
#include "Core/Log.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstring>
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

    // Create transfer buffer for texture data
    Uint32 textureSize = m_width * m_height * 4; // RGBA = 4 bytes per pixel
    SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = textureSize;
    transferBufferInfo.props = 0;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferBufferInfo);
    if (!transferBuffer) {
        SDL_DestroySurface(rgbaSurface);
        throw std::runtime_error("Failed to create transfer buffer for texture upload");
    }

    // Map transfer buffer and copy pixel data
    void* mappedData = SDL_MapGPUTransferBuffer(m_device, transferBuffer, false);
    if (!mappedData) {
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_DestroySurface(rgbaSurface);
        throw std::runtime_error("Failed to map transfer buffer");
    }

    std::memcpy(mappedData, rgbaSurface->pixels, textureSize);
    SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

    // Create command buffer for upload
    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(m_device);
    if (!uploadCmd) {
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
        SDL_DestroySurface(rgbaSurface);
        throw std::runtime_error("Failed to acquire command buffer for texture upload");
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);
    if (copyPass) {
        SDL_GPUTextureTransferInfo transferSrc{};
        transferSrc.transfer_buffer = transferBuffer;
        transferSrc.offset = 0;
        transferSrc.pixels_per_row = m_width;
        transferSrc.rows_per_layer = m_height;

        SDL_GPUTextureRegion textureRegion{};
        textureRegion.texture = m_texture;
        textureRegion.mip_level = 0;
        textureRegion.layer = 0;
        textureRegion.x = 0;
        textureRegion.y = 0;
        textureRegion.z = 0;
        textureRegion.w = m_width;
        textureRegion.h = m_height;
        textureRegion.d = 1;

        SDL_UploadToGPUTexture(copyPass, &transferSrc, &textureRegion, false);
        SDL_EndGPUCopyPass(copyPass);
    }

    SDL_SubmitGPUCommandBuffer(uploadCmd);
    SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);

    SDL_DestroySurface(rgbaSurface);
    LOG_INFO("Loaded texture: {} ({}x{})", path, m_width, m_height);
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
        // Create transfer buffer for pixel data
        Uint32 textureSize = width * height * 4; // RGBA = 4 bytes per pixel
        SDL_GPUTransferBufferCreateInfo transferBufferInfo{};
        transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferInfo.size = textureSize;
        transferBufferInfo.props = 0;

        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferBufferInfo);
        if (transferBuffer) {
            // Map transfer buffer and copy pixel data
            void* mappedData = SDL_MapGPUTransferBuffer(m_device, transferBuffer, false);
            if (mappedData) {
                std::memcpy(mappedData, pixelData, textureSize);
                SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

                // Upload pixel data
                SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(m_device);
                if (uploadCmd) {
                    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);
                    if (copyPass) {
                        SDL_GPUTextureTransferInfo transferSrc{};
                        transferSrc.transfer_buffer = transferBuffer;
                        transferSrc.offset = 0;
                        transferSrc.pixels_per_row = width;
                        transferSrc.rows_per_layer = height;

                        SDL_GPUTextureRegion textureRegion{};
                        textureRegion.texture = m_texture;
                        textureRegion.mip_level = 0;
                        textureRegion.layer = 0;
                        textureRegion.x = 0;
                        textureRegion.y = 0;
                        textureRegion.z = 0;
                        textureRegion.w = width;
                        textureRegion.h = height;
                        textureRegion.d = 1;

                        SDL_UploadToGPUTexture(copyPass, &transferSrc, &textureRegion, false);
                        SDL_EndGPUCopyPass(copyPass);
                    }
                    SDL_SubmitGPUCommandBuffer(uploadCmd);
                }
                SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);
            }
        }
    }

    LOG_INFO("Created texture from pixels ({}x{})", width, height);
}

} // namespace Runa
