// File: src/runapch.h
// Precompiled Header - Common C++ and library headers
// Optimized for frequently used headers across the codebase

#pragma once

// ============================================================================
// C++ Standard Library - Containers (most frequently used)
// ============================================================================
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

// ============================================================================
// C++ Standard Library - Memory Management
// ============================================================================
#include <memory>

// ============================================================================
// C++ Standard Library - Utilities (frequently used)
// ============================================================================
#include <utility>
#include <algorithm>
#include <functional>
#include <optional>
#include <variant>

// ============================================================================
// C++ Standard Library - I/O (commonly used)
// ============================================================================
#include <iostream>
#include <fstream>
#include <sstream>

// ============================================================================
// C++ Standard Library - Errors
// ============================================================================
#include <stdexcept>
#include <exception>

// ============================================================================
// C++ Standard Library - System & Threading
// ============================================================================
#include <chrono>
#include <thread>
#include <filesystem>

// ============================================================================
// C++ Standard Library - C compatibility (for memcpy, etc.)
// ============================================================================
#include <cstring>
#include <cstdint>
#include <cstddef>

// ============================================================================
// Third-party libraries - spdlog (Fast C++ logging - used throughout)
// ============================================================================
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// ============================================================================
// Third-party libraries - SDL3 (Graphics/Window/GPU API - used everywhere)
// ============================================================================
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
