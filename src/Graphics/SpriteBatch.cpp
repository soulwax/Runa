// File: src/Graphics/SpriteBatch.cpp

#include "SpriteBatch.h"
#include "../runapch.h"
#include "Core/Log.h"
#include "Renderer.h"
#include "Shader.h"

namespace Runa {

SpriteBatch::SpriteBatch(Renderer &renderer) : m_renderer(renderer) {
  initializeShader();
  createPipeline();
  createBuffers();
}

SpriteBatch::~SpriteBatch() {
  SDL_GPUDevice *device = m_renderer.getDevice();
  if (device) {
    if (m_pipeline) {
      SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
    }
    if (m_sampler) {
      SDL_ReleaseGPUSampler(device, m_sampler);
    }
    if (m_vertexBuffer) {
      SDL_ReleaseGPUBuffer(device, m_vertexBuffer);
    }
    if (m_indexBuffer) {
      SDL_ReleaseGPUBuffer(device, m_indexBuffer);
    }
  }
}

void SpriteBatch::initializeShader() {
  try {
    // Load textured sprite shader with correct descriptor set (set 2)
    LOG_INFO("Loading textured sprite shader...");
    m_shader = m_renderer.createShader(
        "Resources/shaders/compiled/sprite_fixed.vert.spv",
        "Resources/shaders/compiled/sprite.frag.spv");
    if (!m_shader || !m_shader->isValid()) {
      LOG_WARN("Failed to load textured shader, trying color-only fallback");
      m_shader = m_renderer.createShader(
          "Resources/shaders/compiled/sprite_color.vert.spv",
          "Resources/shaders/compiled/sprite_color.frag.spv");
      if (!m_shader || !m_shader->isValid()) {
        LOG_ERROR("Failed to load any sprite shader!");
      }
    }
  } catch (const std::exception &e) {
    LOG_ERROR("Error initializing sprite shader: {}", e.what());
  }
}

void SpriteBatch::createPipeline() {
  SDL_GPUDevice *device = m_renderer.getDevice();
  if (!device) {
    LOG_ERROR("Cannot create pipeline: device is null");
    return;
  }
  if (!m_shader) {
    LOG_ERROR("Cannot create pipeline: shader is null");
    return;
  }
  if (!m_shader->isValid()) {
    LOG_ERROR("Cannot create pipeline: shader is invalid - vertex or fragment "
              "shader failed to load");
    return;
  }

  LOG_DEBUG("Creating SpriteBatch pipeline with alpha blending...");

  // Create sampler for texture sampling
  SDL_GPUSamplerCreateInfo samplerInfo{};
  samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
  samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
  samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

  m_sampler = SDL_CreateGPUSampler(device, &samplerInfo);
  if (!m_sampler) {
    LOG_ERROR("Failed to create sampler: {}", SDL_GetError());
    return;
  }

  // Create graphics pipeline with alpha blending
  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
  SDL_zero(pipelineInfo);

  // Shaders
  pipelineInfo.vertex_shader = m_shader->getVertexShader();
  pipelineInfo.fragment_shader = m_shader->getFragmentShader();

  // Vertex input state
  SDL_GPUVertexAttribute vertexAttributes[3];
  vertexAttributes[0].location = 0;
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; // Position
  vertexAttributes[0].offset = 0;
  vertexAttributes[0].buffer_slot = 0;

  vertexAttributes[1].location = 1;
  vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; // TexCoord
  vertexAttributes[1].offset = sizeof(float) * 2;
  vertexAttributes[1].buffer_slot = 0;

  vertexAttributes[2].location = 2;
  vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; // Color
  vertexAttributes[2].offset = sizeof(float) * 4;
  vertexAttributes[2].buffer_slot = 0;

  SDL_GPUVertexBufferDescription vertexBufferDesc{};
  vertexBufferDesc.slot = 0;
  vertexBufferDesc.pitch = sizeof(Vertex);
  vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDesc.instance_step_rate = 0;

  SDL_GPUVertexInputState vertexInputState{};
  vertexInputState.vertex_buffer_descriptions = &vertexBufferDesc;
  vertexInputState.num_vertex_buffers = 1;
  vertexInputState.vertex_attributes = vertexAttributes;
  vertexInputState.num_vertex_attributes = 3;

  pipelineInfo.vertex_input_state = vertexInputState;
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  // Rasterizer state
  SDL_GPURasterizerState rasterizerState{};
  rasterizerState.fill_mode = SDL_GPU_FILLMODE_FILL;
  rasterizerState.cull_mode = SDL_GPU_CULLMODE_NONE;
  rasterizerState.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
  rasterizerState.enable_depth_bias = false;
  rasterizerState.enable_depth_clip = false;

  pipelineInfo.rasterizer_state = rasterizerState;

  // Multisample state
  SDL_GPUMultisampleState multisampleState{};
  multisampleState.sample_count = SDL_GPU_SAMPLECOUNT_1;
  multisampleState.sample_mask = 0; // Must be 0 when enable_mask is false
  multisampleState.enable_mask = false;

  pipelineInfo.multisample_state = multisampleState;

  // Depth-stencil state (no depth test for 2D)
  SDL_GPUDepthStencilState depthStencilState{};
  depthStencilState.enable_depth_test = false;
  depthStencilState.enable_depth_write = false;
  depthStencilState.enable_stencil_test = false;

  pipelineInfo.depth_stencil_state = depthStencilState;

  // CRITICAL: Color blend state with ALPHA BLENDING ENABLED
  SDL_GPUColorTargetBlendState blendState{};
  blendState.enable_blend = true; // Enable alpha blending
  blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  blendState.color_blend_op = SDL_GPU_BLENDOP_ADD;
  blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
  blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  blendState.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  blendState.color_write_mask =
      SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
      SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;

  SDL_GPUColorTargetDescription colorTarget{};
  colorTarget.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; // Swapchain format
  colorTarget.blend_state = blendState;

  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = &colorTarget;
  pipelineInfo.target_info.has_depth_stencil_target = false;

  m_pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
  if (!m_pipeline) {
    LOG_ERROR("Failed to create graphics pipeline: {}", SDL_GetError());
    return;
  }

  LOG_INFO(
      "SpriteBatch graphics pipeline created successfully with alpha blending");
}

void SpriteBatch::createBuffers() {
  SDL_GPUDevice *device = m_renderer.getDevice();
  if (!device) {
    return;
  }

  // Create vertex buffer (6 vertices per sprite for 2 triangles)
  SDL_GPUBufferCreateInfo vertexBufferInfo{};
  vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  vertexBufferInfo.size = sizeof(Vertex) * 6 * m_maxSprites;

  m_vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo);
  if (!m_vertexBuffer) {
    LOG_ERROR("Failed to create vertex buffer: {}", SDL_GetError());
  }
}

void SpriteBatch::begin() {
  if (m_inBatch) {
    LOG_WARN("SpriteBatch::begin() called while already in batch!");
    return;
  }
  m_inBatch = true;
  m_drawCalls.clear();
}

void SpriteBatch::draw(const Texture &texture, int x, int y, int srcX, int srcY,
                       int srcWidth, int srcHeight, float r, float g, float b,
                       float a, float scaleX, float scaleY) {
  if (!m_inBatch) {
    LOG_WARN("SpriteBatch::draw() called outside of begin/end!");
    return;
  }

  SpriteDrawCall call;
  call.texture = &texture;
  call.x = x;
  call.y = y;
  call.srcX = srcX;
  call.srcY = srcY;
  call.srcWidth = srcWidth;
  call.srcHeight = srcHeight;
  call.r = r;
  call.g = g;
  call.b = b;
  call.a = a;
  call.scaleX = scaleX;
  call.scaleY = scaleY;

  m_drawCalls.push_back(call);
}

void SpriteBatch::draw(const Texture &texture, int x, int y,
                       const SpriteFrame &frame, float r, float g, float b,
                       float a, float scaleX, float scaleY) {
  draw(texture, x, y, frame.x, frame.y, frame.width, frame.height, r, g, b, a,
       scaleX, scaleY);
}

void SpriteBatch::draw(const Texture &texture, int x, int y, float r, float g,
                       float b, float a, float scaleX, float scaleY) {
  draw(texture, x, y, 0, 0, texture.getWidth(), texture.getHeight(), r, g, b, a,
       scaleX, scaleY);
}

void SpriteBatch::end() {
  if (!m_inBatch) {
    LOG_WARN("SpriteBatch::end() called without begin!");
    return;
  }

  if (m_drawCalls.empty()) {
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  SDL_GPUDevice *device = m_renderer.getDevice();
  if (!device) {
    LOG_ERROR("SpriteBatch::end(): GPU device is null!");
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }
  if (!m_pipeline) {
    LOG_ERROR("SpriteBatch::end(): Pipeline is null! (shader loading or "
              "pipeline creation failed)");
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }
  if (!m_sampler) {
    LOG_ERROR("SpriteBatch::end(): Sampler is null!");
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  // Build all vertex data for all sprites
  std::vector<Vertex> allVertices;
  allVertices.reserve(m_drawCalls.size() * 6);

  for (const auto &call : m_drawCalls) {
    if (!call.texture || !call.texture->isValid()) {
      continue;
    }

    // Calculate sprite quad vertices
    float x1 = static_cast<float>(call.x);
    float y1 = static_cast<float>(call.y);
    float x2 = x1 + (call.srcWidth * call.scaleX);
    float y2 = y1 + (call.srcHeight * call.scaleY);

    // Calculate texture coordinates
    float texWidth = static_cast<float>(call.texture->getWidth());
    float texHeight = static_cast<float>(call.texture->getHeight());
    float u1 = call.srcX / texWidth;
    float v1 = call.srcY / texHeight;
    float u2 = (call.srcX + call.srcWidth) / texWidth;
    float v2 = (call.srcY + call.srcHeight) / texHeight;

    // DEBUG: Log UV coords for first sprite
    static bool logged = false;
    if (!logged) {
        LOG_INFO("Sprite UV coords - u1:{} v1:{} u2:{} v2:{} (tex:{}x{}, src:{},{} {}x{})",
            u1, v1, u2, v2, texWidth, texHeight, call.srcX, call.srcY, call.srcWidth, call.srcHeight);
        logged = true;
    }

    // Add 6 vertices for 2 triangles (quad)
    allVertices.push_back(
        {x1, y1, u1, v1, call.r, call.g, call.b, call.a}); // Top-left
    allVertices.push_back(
        {x2, y1, u2, v1, call.r, call.g, call.b, call.a}); // Top-right
    allVertices.push_back(
        {x1, y2, u1, v2, call.r, call.g, call.b, call.a}); // Bottom-left
    allVertices.push_back(
        {x2, y1, u2, v1, call.r, call.g, call.b, call.a}); // Top-right
    allVertices.push_back(
        {x2, y2, u2, v2, call.r, call.g, call.b, call.a}); // Bottom-right
    allVertices.push_back(
        {x1, y2, u1, v2, call.r, call.g, call.b, call.a}); // Bottom-left
  }

  if (allVertices.empty()) {
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  // Upload vertex data to GPU
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferInfo.size = allVertices.size() * sizeof(Vertex);

  SDL_GPUTransferBuffer *transferBuffer =
      SDL_CreateGPUTransferBuffer(device, &transferInfo);
  if (!transferBuffer) {
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  void *data = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
  if (!data) {
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  std::memcpy(data, allVertices.data(), allVertices.size() * sizeof(Vertex));
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  // Acquire command buffer
  SDL_GPUCommandBuffer *cmdBuffer = m_renderer.acquireCommandBuffer();
  if (!cmdBuffer) {
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  // Upload transfer buffer to vertex buffer
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
  if (copyPass) {
    SDL_GPUTransferBufferLocation transferLoc{};
    transferLoc.transfer_buffer = transferBuffer;
    transferLoc.offset = 0;

    SDL_GPUBufferRegion bufferRegion{};
    bufferRegion.buffer = m_vertexBuffer;
    bufferRegion.offset = 0;
    bufferRegion.size = allVertices.size() * sizeof(Vertex);

    SDL_UploadToGPUBuffer(copyPass, &transferLoc, &bufferRegion, false);
    SDL_EndGPUCopyPass(copyPass);
  }

  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

  // Acquire swapchain texture
  SDL_GPUTexture *swapchainTexture = nullptr;
  if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer,
                                      m_renderer.getWindow().getHandle(),
                                      &swapchainTexture, nullptr, nullptr)) {
    SDL_CancelGPUCommandBuffer(cmdBuffer);
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  if (!swapchainTexture) {
    SDL_CancelGPUCommandBuffer(cmdBuffer);
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  // Determine load operation (clear or load)
  SDL_GPULoadOp loadOp = SDL_GPU_LOADOP_LOAD;
  if (m_renderer.needsClear()) {
    loadOp = SDL_GPU_LOADOP_CLEAR;
    m_renderer.clearApplied();
  }

  // Begin render pass with alpha blending support
  SDL_GPUColorTargetInfo colorTarget{};
  colorTarget.texture = swapchainTexture;
  auto clearColor = m_renderer.getClearColor();
  colorTarget.clear_color.r = clearColor.r;
  colorTarget.clear_color.g = clearColor.g;
  colorTarget.clear_color.b = clearColor.b;
  colorTarget.clear_color.a = clearColor.a;
  colorTarget.load_op = loadOp;
  colorTarget.store_op = SDL_GPU_STOREOP_STORE;

  SDL_GPURenderPass *renderPass =
      SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
  if (!renderPass) {
    SDL_CancelGPUCommandBuffer(cmdBuffer);
    m_drawCalls.clear();
    m_inBatch = false;
    return;
  }

  // Bind pipeline
  SDL_BindGPUGraphicsPipeline(renderPass, m_pipeline);

  // NOTE: Fixed shader has hardcoded screen size, no push constants needed
  // float screenSize[2] = {1280.0f, 720.0f};
  // SDL_PushGPUVertexUniformData(cmdBuffer, 0, screenSize, sizeof(screenSize));

  // Bind vertex buffer
  SDL_GPUBufferBinding vertexBinding{};
  vertexBinding.buffer = m_vertexBuffer;
  vertexBinding.offset = 0;
  SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

  // Render each sprite
  Uint32 vertexOffset = 0;
  for (const auto &call : m_drawCalls) {
    if (!call.texture || !call.texture->isValid()) {
      continue;
    }

    // Bind texture sampler (now properly supported with num_samplers in shader)
    SDL_GPUTextureSamplerBinding textureBinding;
    textureBinding.texture = call.texture->getHandle();
    textureBinding.sampler = m_sampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);

    // Draw the sprite (6 vertices)
    SDL_DrawGPUPrimitives(renderPass, 6, 1, vertexOffset, 0);
    vertexOffset += 6;
  }

  SDL_EndGPURenderPass(renderPass);
  SDL_SubmitGPUCommandBuffer(cmdBuffer);

  m_drawCalls.clear();
  m_inBatch = false;
}

void SpriteBatch::renderSprite(const SpriteDrawCall &call,
                               SDL_GPUCommandBuffer *cmdBuffer,
                               SDL_GPURenderPass *renderPass,
                               SDL_GPUTexture *swapchainTexture) {
  // This method is no longer used - rendering is now done in batch in end()
}

} // namespace Runa
