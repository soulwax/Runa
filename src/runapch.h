// File: src/runapch.h
// Precompiled Header - Common C++ and library headers
// Optimized for frequently used headers across the codebase

#pragma once

// C++ Standard Library - Containers (most frequently used)
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

// C++ Standard Library - Memory Management
#include <memory>
// C++ Standard Library - Utilities (frequently used)
#include <utility>
#include <algorithm>
#include <functional>
#include <optional>
#include <variant>

// C++ Standard Library - I/O (commonly used)
#include <fstream>
#include <sstream>
// Note: <iostream> removed - use spdlog instead

// C++ Standard Library - Errors
#include <stdexcept>
#include <exception>

// C++ Standard Library - Time (used for timing/delta time)
#include <chrono>

// C++ Standard Library - C compatibility (for memcpy, etc.)
#include <cstring>
#include <cstdint>

// Third-party libraries
// spdlog - Fast C++ logging library (used throughout)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// SDL3 - Core headers (used in most graphics code)
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
