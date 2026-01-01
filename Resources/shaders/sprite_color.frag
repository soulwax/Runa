# File: Resources/shaders/sprite_color.frag
# version 450

// Minimal fragment shader - just color output, no texture sampling
layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}
