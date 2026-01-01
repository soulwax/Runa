# File: Resources/shaders/sprite_debug.frag
# version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

void main() {
    // Sample the texture
    vec4 texColor = texture(texSampler, fragTexCoord);

    // DEBUG: Try different outputs to diagnose the issue

    // Test 1: Output magenta to verify shader is being used
    // outColor = vec4(1.0, 0.0, 1.0, 1.0);

    // Test 2: Output UV coords as colors (should show gradient)
    // outColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);

    // Test 3: Output texture color directly (what we want to work)
    outColor = texColor;

    // Test 4: Output texture color with debug tint
    // outColor = vec4(texColor.rgb, 1.0);
}
