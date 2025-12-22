#!/bin/bash
# File: compile_shaders.sh

if ! command -v glslc &> /dev/null; then
    echo "Error: glslc not found. Please install the Vulkan SDK."
    echo "Download from: https://vulkan.lunarg.com/sdk/home"
    exit 1
fi

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SHADER_DIR="$SCRIPT_DIR/Resources/shaders"
COMPILED_DIR="$SHADER_DIR/compiled"

if [ ! -d "$SHADER_DIR" ]; then
    echo "Error: Shader directory not found: $SHADER_DIR"
    exit 1
fi

# Create compiled directory if it doesn't exist
mkdir -p "$COMPILED_DIR"

echo "Compiling shaders to SPIR-V..."
echo "Shader directory: $SHADER_DIR"
echo "Output directory: $COMPILED_DIR"

cd "$SHADER_DIR"

# Compile vertex shaders
for file in *.vert; do
    if [ -f "$file" ]; then
        output="compiled/${file%.vert}.vert.spv"
        echo "Compiling $file -> $output"
        glslc -fshader-stage=vertex "$file" -o "$output"
    fi
done

# Compile fragment shaders
for file in *.frag; do
    if [ -f "$file" ]; then
        output="compiled/${file%.frag}.frag.spv"
        echo "Compiling $file -> $output"
        glslc -fshader-stage=fragment "$file" -o "$output"
    fi
done

echo "Shader compilation complete!"
