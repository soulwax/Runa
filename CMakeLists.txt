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

# Define export macro for engine DLL
target_compile_definitions(Runa2Engine PRIVATE RUNA2_ENGINE_EXPORTS)

# Link libraries to engine
# SDL3 and SDL3_image are PUBLIC so executables can access headers
target_link_libraries(Runa2Engine PRIVATE
    yaml-cpp::yaml-cpp
    spdlog::spdlog
    PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
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
add_executable(Runa2
    src/main.cpp
)

# Link game executable to engine DLL
# Also link spdlog since Log.h includes runapch.h which needs spdlog
target_link_libraries(Runa2 PRIVATE
    Runa2Engine
    spdlog::spdlog
)

# Include directories for game
target_include_directories(Runa2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable debug symbols for Debug builds
target_compile_options(Runa2 PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
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
