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

# Fetch SDL3
include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main  # Use 'main' for latest, or a specific tag like 'release-3.2.0'
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

# Add executable
add_executable(Runa2
    src/main.cpp

    # Core
    src/Core/Application.cpp
    src/Core/Application.h

    # Graphics
    src/Graphics/Window.cpp
    src/Graphics/Window.h
    src/Graphics/Renderer.cpp
    src/Graphics/Renderer.h
    src/Graphics/Shader.cpp
    src/Graphics/Shader.h
)

# Link SDL3
target_link_libraries(Runa2 PRIVATE SDL3::SDL3)

# Include directories
target_include_directories(Runa2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable debug symbols for Debug builds
target_compile_options(Runa2 PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)
