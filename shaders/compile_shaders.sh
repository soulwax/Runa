#!/bin/bash
# File: shaders/compile_shaders.sh

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
