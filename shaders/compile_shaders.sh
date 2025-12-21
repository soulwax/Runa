#!/bin/bash
# Shader compilation script for Windows using glslc (part of Vulkan SDK)
# This script compiles GLSL shaders to SPIR-V format for use with SDL3 GPU

# Check if glslc is available
if ! command -v glslc &> /dev/null; then
    echo "Error: glslc not found. Please install the Vulkan SDK."
    echo "Download from: https://vulkan.lunarg.com/sdk/home"
    exit 1
fi

echo "Compiling shaders to SPIR-V..."

# Compile vertex shaders
for file in *.vert; do
    if [ -f "$file" ]; then
        output="${file%.vert}.vert.spv"
        echo "Compiling $file -> $output"
        glslc -fshader-stage=vertex "$file" -o "$output"
    fi
done

# Compile fragment shaders
for file in *.frag; do
    if [ -f "$file" ]; then
        output="${file%.frag}.frag.spv"
        echo "Compiling $file -> $output"
        glslc -fshader-stage=fragment "$file" -o "$output"
    fi
done

echo "Shader compilation complete!"
