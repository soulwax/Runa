#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D screenTexture;

layout(push_constant) uniform PushConstants {
    vec2 screenSize;
    float time;
} pc;

void main() {
    vec2 uv = fragTexCoord;
    
    // Create multiple wave layers for realistic water effect
    float waveSpeed1 = 1.0;
    float waveSpeed2 = 0.7;
    float waveSpeed3 = 1.3;
    
    float waveFrequency1 = 8.0;
    float waveFrequency2 = 12.0;
    float waveFrequency3 = 6.0;
    
    float waveAmplitude1 = 0.01;
    float waveAmplitude2 = 0.008;
    float waveAmplitude3 = 0.012;
    
    // Wave 1: Horizontal waves
    float wave1 = sin(uv.y * waveFrequency1 + pc.time * waveSpeed1) * waveAmplitude1;
    
    // Wave 2: Vertical waves
    float wave2 = cos(uv.x * waveFrequency2 + pc.time * waveSpeed2) * waveAmplitude2;
    
    // Wave 3: Diagonal waves
    float wave3 = sin((uv.x + uv.y) * waveFrequency3 + pc.time * waveSpeed3) * waveAmplitude3;
    
    // Combine waves
    vec2 distortion = vec2(wave1 + wave3, wave2 + wave3);
    
    // Add circular ripples
    vec2 center = vec2(0.5, 0.5);
    vec2 dir = uv - center;
    float dist = length(dir);
    
    // Animated ripple
    float rippleSpeed = 2.0;
    float rippleFrequency = 15.0;
    float rippleDecay = 1.0 / (1.0 + dist * 5.0);
    float ripple = sin(dist * rippleFrequency - pc.time * rippleSpeed) * rippleDecay * 0.005;
    
    vec2 rippleDistortion = normalize(dir) * ripple;
    distortion += rippleDistortion;
    
    // Apply distortion to UV coordinates
    vec2 distortedUV = uv + distortion;
    
    // Clamp to avoid sampling outside texture
    distortedUV = clamp(distortedUV, vec2(0.001), vec2(0.999));
    
    // Sample the screen texture with distortion
    vec4 color = texture(screenTexture, distortedUV);
    
    // Add slight color tint (cyan/blue water effect)
    vec3 waterTint = vec3(0.9, 1.0, 1.05);
    color.rgb *= mix(vec3(1.0), waterTint, 0.15);
    
    // Add subtle brightness variation based on wave height
    float brightnessVariation = sin(dist * 10.0 + pc.time * 1.5) * 0.05 + 1.0;
    color.rgb *= brightnessVariation;
    
    outColor = color;
}

