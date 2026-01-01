# File: Resources/shaders/kaleidoscope.frag
# version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D screenTexture;

layout(push_constant) uniform PushConstants {
    vec2 screenSize;
    float time;
} pc;

void main() {
    vec2 uv = fragTexCoord;
    vec2 center = vec2(0.5, 0.5);
    
    // Move origin to center
    vec2 coord = uv - center;
    
    // Convert to polar coordinates
    float angle = atan(coord.y, coord.x);
    float radius = length(coord);
    
    // Number of segments (sides of the kaleidoscope)
    float segments = 6.0;
    
    // Rotate the pattern over time
    angle += pc.time * 0.5;
    
    // Mirror the angle to create symmetry
    angle = mod(angle, 2.0 * 3.14159 / segments);
    if (angle > 3.14159 / segments) {
        angle = 2.0 * 3.14159 / segments - angle;
    }
    
    // Convert back to cartesian
    vec2 mirroredCoord = vec2(cos(angle), sin(angle)) * radius;
    vec2 mirroredUV = mirroredCoord + center;
    
    // Clamp to avoid sampling outside texture
    mirroredUV = clamp(mirroredUV, vec2(0.001), vec2(0.999));
    
    // Sample the screen texture
    vec4 color = texture(screenTexture, mirroredUV);
    
    // Add radial color shift for extra visual interest
    float colorShift = sin(radius * 10.0 - pc.time * 2.0) * 0.1;
    color.rgb = mix(color.rgb, 
                    vec3(color.r, color.g + colorShift, color.b - colorShift), 
                    0.3);
    
    // Vignette
    float vignette = 1.0 - smoothstep(0.3, 0.8, radius);
    color.rgb *= vignette;
    
    outColor = color;
}
