#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

void main() {
    // Hardcoded screen size for testing (1280x720)
    vec2 screenSize = vec2(1280.0, 720.0);

    // Convert from pixel coordinates to normalized device coordinates
    vec2 normalizedPos = (inPosition / screenSize) * 2.0 - 1.0;
    normalizedPos.y = -normalizedPos.y; // Flip Y axis

    gl_Position = vec4(normalizedPos, 0.0, 1.0);
    fragTexCoord = inTexCoord;
    fragColor = inColor;
}
