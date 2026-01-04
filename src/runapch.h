// File: src/runapch.h
// Precompiled Header - Common C++ and library headers
// Optimized for frequently used headers across the codebase

#ifndef RUNA_RUNAPCH_H
#define RUNA_RUNAPCH_H

// ============================================================================
// C++ Standard Library - Containers (most frequently used)
// ============================================================================
#include <array>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ============================================================================
// C++ Standard Library - Memory Management
// ============================================================================
#include <memory>

// ============================================================================
// C++ Standard Library - Utilities (frequently used)
// ============================================================================
#include <algorithm>
#include <functional>
#include <optional>
#include <utility>
#include <variant>

// ============================================================================
// C++ Standard Library - I/O (commonly used)
// ============================================================================
#include <fstream>
#include <iostream>
#include <sstream>

// ============================================================================
// C++ Standard Library - Errors
// ============================================================================
#include <exception>
#include <stdexcept>

// ============================================================================
// C++ Standard Library - System & Threading
// ============================================================================
#include <chrono>
#include <filesystem>
#include <thread>

// ============================================================================
// C++ Standard Library - C compatibility (for memcpy, etc.)
// ============================================================================
#include <cstddef>
#include <cstdint>
#include <cstring>

// ============================================================================
// Third-party libraries - spdlog (Fast C++ logging - used throughout)
// ============================================================================
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// ============================================================================
// Third-party libraries - SDL3 (Graphics/Window/GPU API - used everywhere)
// ============================================================================
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#endif // RUNA_RUNAPCH_H
