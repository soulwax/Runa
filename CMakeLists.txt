# File: CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

project(Runa2
    VERSION 1.0.0
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch dependencies
include(FetchContent)

# SDL3
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

# SDL3_image
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

# yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0
    GIT_SHALLOW TRUE
)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3 SDL3_image yaml-cpp spdlog)

# Add executable
add_executable(Runa2
    src/runapch.cpp
    src/runapch.h
    src/main.cpp

    # Core
    src/Core/Application.cpp
    src/Core/Application.h
    src/Core/Log.cpp
    src/Core/Log.h
    src/Core/ResourceManager.cpp
    src/Core/ResourceManager.h

    # Graphics
    src/Graphics/Window.cpp
    src/Graphics/Window.h
    src/Graphics/Renderer.cpp
    src/Graphics/Renderer.h
    src/Graphics/Shader.cpp
    src/Graphics/Shader.h
    src/Graphics/Texture.cpp
    src/Graphics/Texture.h
    src/Graphics/SpriteSheet.cpp
    src/Graphics/SpriteSheet.h
    src/Graphics/SpriteBatch.cpp
    src/Graphics/SpriteBatch.h
    src/Graphics/TileMap.cpp
    src/Graphics/TileMap.h
)

# Link libraries
target_link_libraries(Runa2 PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    yaml-cpp::yaml-cpp
    spdlog::spdlog
)

# Include directories
target_include_directories(Runa2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable debug symbols for Debug builds
target_compile_options(Runa2 PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)
