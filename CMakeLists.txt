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

# SDL3_ttf
FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
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

# yaml-cpp build options
# Note: YAML_BUILD_SHARED_LIBS intentionally lacks the CPP prefix to avoid
# conflicts with CMake's standard BUILD_SHARED_LIBS variable. This is the
# official variable name as defined in yaml-cpp's CMakeLists.txt.
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

# EnTT - Entity Component System
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
    GIT_SHALLOW TRUE
)

# SDL3_ttf defaults to using *system* FreeType unless MSVC/Android. On MinGW this
# commonly fails (no package manager integration), so force vendored deps.
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)

# SDL3_image: Disable problematic formats to avoid Git submodule issues
# We only need PNG/JPG support for basic sprite loading
set(SDL3IMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_WEBP OFF CACHE BOOL "" FORCE)

# Make dependencies available
# Note: All dependencies are configured as static libraries, so they will use
# their own default output directories. We avoid setting global CMAKE_*_OUTPUT_DIRECTORY
# variables with FORCE here, as that would override dependency defaults and is unnecessary
# since our own targets (Runa2Engine and Runa2) have explicit output directory settings.
FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf yaml-cpp spdlog EnTT)

# ============================================================================
# Engine Library (DLL)
# ============================================================================

# Add shared library for engine
add_library(Runa2Engine SHARED
    src/runapch.cpp
    src/runapch.h
    src/RunaAPI.h

    # Core
    src/Core/Application.cpp
    src/Core/Application.h
    src/Core/Log.cpp
    src/Core/Log.h
    src/Core/ResourceManager.cpp
    src/Core/ResourceManager.h
    src/Core/Input.cpp
    src/Core/Input.h
    src/Core/Entity.cpp
    src/Core/Entity.h
    src/Core/PlayerController.cpp
    src/Core/PlayerController.h
    src/Core/Collision.cpp
    src/Core/Collision.h

    # ECS
    src/ECS/Components.h
    src/ECS/Systems.cpp
    src/ECS/Systems.h
    src/ECS/Registry.cpp
    src/ECS/Registry.h

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
    src/Graphics/PostProcess.cpp
    src/Graphics/PostProcess.h
    src/Graphics/Font.cpp
    src/Graphics/Font.h
    src/Graphics/Camera.cpp
    src/Graphics/Camera.h
)

# Define export macro for engine DLL
target_compile_definitions(Runa2Engine PRIVATE
    RUNA2_ENGINE_EXPORTS
    SPDLOG_FMT_RUNTIME_CHECKS=0  # Disable compile-time format string checking
)

# Link libraries to engine
# SDL3, SDL3_image, SDL3_ttf, and EnTT are PUBLIC so executables can access headers
target_link_libraries(Runa2Engine PRIVATE
    yaml-cpp::yaml-cpp
    spdlog::spdlog
    PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    EnTT::EnTT
)

# Include directories for engine (PUBLIC so executables linking to it get these includes)
target_include_directories(Runa2Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable debug symbols for Debug builds
target_compile_options(Runa2Engine PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# Set output directory for DLL (so executable can find it)
set_target_properties(Runa2Engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

# ============================================================================
# Game Executable
# ============================================================================

# Add executable (only game logic)
# Using grass_test.cpp for texture debugging
add_executable(Runa2
    src/grass_test.cpp
    # src/main.cpp  # Disabled - uses ECS
)

# Link game executable to engine DLL
# Also link spdlog since Log.h includes runapch.h which needs spdlog
target_link_libraries(Runa2 PRIVATE
    Runa2Engine
    spdlog::spdlog
)

# Define spdlog options for game executable
target_compile_definitions(Runa2 PRIVATE
    SPDLOG_FMT_RUNTIME_CHECKS=0  # Disable compile-time format string checking
)

# Include directories for game
target_include_directories(Runa2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable debug symbols for Debug builds
target_compile_options(Runa2 PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# Set output directory for executable (keep it in build root with Runa2Engine.dll)
set_target_properties(Runa2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

# Function to copy DLLs to executable directory
function(copy_dlls_to_exe target)
    # Copy engine DLL
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Runa2Engine>
            $<TARGET_FILE_DIR:${target}>
        COMMENT "Copying Runa2Engine.dll"
    )
    
    # Copy SDL3 DLLs
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:${target}>
        COMMENT "Copying SDL3.dll"
    )
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:${target}>
        COMMENT "Copying SDL3_image.dll"
    )
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
            $<TARGET_FILE_DIR:${target}>
        COMMENT "Copying SDL3_ttf.dll"
    )
    
    # Copy spdlog DLL - check if it exists as a shared library
    # spdlog may be built as shared despite SPDLOG_BUILD_SHARED OFF in some cases
    if(TARGET spdlog::spdlog)
        get_target_property(spdlog_type spdlog::spdlog TYPE)
        if(spdlog_type STREQUAL "SHARED_LIBRARY")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:spdlog::spdlog>
                    $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying spdlog DLL"
            )
        endif()
    endif()
endfunction()

# Copy all required DLLs
copy_dlls_to_exe(Runa2)
