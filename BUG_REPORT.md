# C++ Bug Hunt Report - Runa2 Engine

## Critical Bugs Found

### 1. **SDL_Init/TTF_Init Documentation Error** ⚠️
**File**: `BUG_REPORT.md:16` (this file)
**Severity**: Low - Misleading documentation (code is actually correct)

**Issue**: This bug report incorrectly states that `SDL_Init()` returns `0` on success and non-zero on failure (SDL2 behavior). However, **SDL3 returns `bool`** (`true` on success, `false` on failure).

**Actual Code** (which is CORRECT for SDL3):
```cpp
// SDL3 returns bool: true on success, false on failure
if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    throw std::runtime_error(...);
}
```

**Analysis**:
- When SDL3 initializes successfully (returns `true`), `!true` = `false`, so it doesn't throw ✓
- When SDL3 fails (returns `false`), `!false` = `true`, so it throws ✓

**Status**: The code in `Application.cpp` is **correct for SDL3**. This documentation was written based on SDL2 API semantics and is misleading. The code should NOT be "fixed" as described here.

**Note**: Same applies to `TTF_Init()` - SDL3_ttf also returns `bool` (`true` on success, `false` on failure).

---

### 2. **Double beginFrame() Call** ⚠️
**File**: `src/Graphics/Window.cpp:42` and `src/Core/Application.cpp:119`
**Severity**: Medium - Causes input state to be cleared twice per frame

**Issue**:
- `Window::processEvents()` calls `m_input->beginFrame()` (line 42)
- `Application::mainLoop()` also calls `m_input->beginFrame()` (line 119)
- This causes input "just pressed/released" states to be cleared twice, potentially causing input to be missed

**Problem**: The input system's one-shot states (`m_keysPressed`, `m_keysReleased`, etc.) are cleared twice per frame, which means:
1. First clear happens in `Window::processEvents()`
2. Events are processed and states are set
3. Second clear happens in `Application::mainLoop()`, clearing states that were just set

**Fix**: Remove the `beginFrame()` call from `Window::processEvents()` since `Application::mainLoop()` already handles it:

```cpp
// In Window::processEvents(), remove line 42:
// if (m_input) {
//     m_input->beginFrame();  // REMOVE THIS
// }
```

**Note**: The comment on line 40 says "Clear per-frame input state" but this is redundant since Application already does it.

---

### 3. **Window Move Operations - Dangling Pointer Risk** ⚠️
**File**: `src/Graphics/Window.h:24-25`
**Severity**: Medium - Potential undefined behavior

**Issue**:
```cpp
// Enable moving
Window(Window&&) noexcept = default;
Window& operator=(Window&&) noexcept = default;
```

**Problem**: `Window` contains a raw pointer `Input* m_input` (line 43). When a `Window` is moved:
- The moved-from object's `m_input` pointer still points to the same `Input` object
- If the moved-from object is destroyed, `m_input` becomes dangling
- However, since `Window` is typically not moved in practice, this may not cause immediate issues

**Fix**: Either disable move operations or properly handle the pointer:

```cpp
// Option 1: Disable moving (safer)
Window(Window&&) = delete;
Window& operator=(Window&&) = delete;

// Option 2: Properly handle the pointer (if moving is needed)
Window(Window&& other) noexcept
    : m_window(other.m_window),
      m_width(other.m_width),
      m_height(other.m_height),
      m_shouldClose(other.m_shouldClose),
      m_input(other.m_input) {
    other.m_input = nullptr;  // Clear moved-from pointer
}
```

**Recommendation**: Disable move operations since `Window` is typically created once and not moved.

---

### 4. **Unnecessary const_cast in ResourceManager** ⚠️
**File**: `src/Core/ResourceManager.cpp:16`
**Severity**: Low - Code smell, but likely harmless

**Issue**:
```cpp
const char* basePath = SDL_GetBasePath();
// ...
SDL_free(const_cast<char*>(basePath));
```

**Problem**:
- `SDL_GetBasePath()` in SDL3 returns `char*` (not `const char*`)
- The variable is declared as `const char*` unnecessarily
- The `const_cast` is then needed to free it

**Fix**: Remove the const qualifier:

```cpp
char* basePath = SDL_GetBasePath();
std::filesystem::path currentPath = basePath ? basePath : std::filesystem::current_path();
SDL_free(basePath);  // No const_cast needed
```

---

### 5. **Potential Division by Zero in Camera** ⚠️
**File**: `src/Graphics/Camera.cpp:79, 88-89`
**Severity**: Low - Unlikely but possible

**Issue**:
```cpp
worldX = (screenX - halfWidth) / m_zoom + m_x;
worldY = (screenY - halfHeight) / m_zoom + m_y;
// ...
float worldHalfWidth = halfWidth / m_zoom;
float worldHalfHeight = halfHeight / m_zoom;
```

**Problem**: If `m_zoom` is ever set to 0.0f (which shouldn't happen due to clamping in `zoom()`, but could happen via `setZoom()`), division by zero occurs.

**Fix**: Add safety check in `setZoom()`:

```cpp
void Camera::setZoom(float zoom) {
    m_zoom = (zoom > 0.001f) ? zoom : 0.001f;  // Prevent division by zero
    if (m_zoom < 0.25f) m_zoom = 0.25f;
    if (m_zoom > 4.0f) m_zoom = 4.0f;
}
```

Or add checks in the division operations:

```cpp
float safeZoom = (m_zoom > 0.001f) ? m_zoom : 0.001f;
worldX = (screenX - halfWidth) / safeZoom + m_x;
```

---

### 6. **Potential Memory Leak in Texture Creation** ⚠️
**File**: `src/Graphics/Texture.cpp:71-93`
**Severity**: Medium - Resource leak on error path

**Issue**:
```cpp
void Texture::createFromPixels(int width, int height, const void* pixelData) {
    // ...
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
}
```

**Problem**: If `vk2dTextureLoadFromImage(image)` fails (returns nullptr), the code throws an exception but:
- The `image` is freed AFTER the check, so if `m_texture` is null, `image` is still freed (this is actually correct)
- However, if `vk2dImageFromPixels` succeeds but `vk2dTextureLoadFromImage` fails, we free the image and throw, which is correct

**Actually, this looks correct!** The image is freed regardless of whether texture creation succeeds. But let me verify the logic flow is exception-safe.

**Re-evaluation**: The code appears correct. The image is always freed, and if texture creation fails, we throw after freeing. This is safe.

**However**, there's a potential issue: If `vk2dTextureLoadFromImage` takes ownership of the image and we free it anyway, that could be a double-free. Or if it doesn't take ownership and we forget to free it, that's a leak. Need to verify Vulkan2D API semantics.

**Recommendation**: Verify Vulkan2D documentation to confirm whether `vk2dTextureLoadFromImage` takes ownership of the image or if we need to free it ourselves.

---

## Additional Code Quality Issues

### 7. **TTF_Init Documentation Error** ⚠️
**File**: `BUG_REPORT.md:200` (this file)
**Severity**: Low - Misleading documentation (code is actually correct)

**Issue**: This bug report incorrectly states that `TTF_Init()` returns `0` on success and non-zero on failure (SDL2 behavior). However, **SDL3_ttf returns `bool`** (`true` on success, `false` on failure).

**Actual Code** (which is CORRECT for SDL3):
```cpp
// SDL3_ttf returns bool: true on success, false on failure
if (!TTF_Init()) {
    LOG_WARN("Failed to initialize SDL_ttf: {}", SDL_GetError());
}
```

**Analysis**:
- When SDL3_ttf initializes successfully (returns `true`), `!true` = `false`, so it doesn't log ✓
- When SDL3_ttf fails (returns `false`), `!false` = `true`, so it logs a warning ✓

**Status**: The code in `Application.cpp` is **correct for SDL3**. This documentation was written based on SDL2 API semantics and is misleading. The code should NOT be "fixed" as described here.

---

### 8. **EntityRegistry::getEntityCount() Not Implemented**
**File**: `src/ECS/Registry.h:70`
**Severity**: Low - Incomplete implementation

**Issue**:
```cpp
size_t getEntityCount() const { return 0; /* TODO: Fix for EnTT version */ }
```

**Problem**: This function always returns 0, which is misleading.

**Fix**: Use EnTT's registry size:
```cpp
size_t getEntityCount() const { return m_registry.size(); }
```

---

## Summary

| Bug # | Severity | File | Line | Status |
|-------|----------|------|------|--------|
| 1 | Critical | Application.cpp | 15 | SDL_Init logic inverted |
| 2 | Medium | Window.cpp | 42 | Double beginFrame() call |
| 3 | Medium | Window.h | 24-25 | Move operations with raw pointer |
| 4 | Low | ResourceManager.cpp | 16 | Unnecessary const_cast |
| 5 | Low | Camera.cpp | 79, 88-89 | Potential division by zero |
| 6 | Medium | Texture.cpp | 71-93 | Verify Vulkan2D API ownership |
| 7 | Low | Application.cpp | 23 | TTF_Init logic inverted |
| 8 | Low | Registry.h | 70 | Unimplemented function |

## Recommended Fix Priority

1. **Fix Bug #1 immediately** - SDL initialization is broken
2. **Fix Bug #2** - Input system double-clearing causes missed inputs
3. **Fix Bug #7** - Same pattern as Bug #1
4. **Fix Bug #3** - Prevent potential undefined behavior
5. **Verify Bug #6** - Check Vulkan2D API documentation
6. **Fix remaining low-priority issues**

---

## Testing Recommendations

After fixing these bugs, test:
1. SDL initialization (Bug #1) - Verify window opens correctly
2. Input system (Bug #2) - Verify key presses are detected correctly
3. Window operations (Bug #3) - Verify no crashes on window operations
4. Texture loading (Bug #6) - Verify no memory leaks or crashes
5. Camera zoom (Bug #5) - Test extreme zoom values
