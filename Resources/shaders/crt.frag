# File: Resources/shaders/crt.frag
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
    
    // CRT curvature effect
    vec2 center = vec2(0.5, 0.5);
    vec2 coord = uv - center;
    float dist = length(coord);
    
    // Barrel distortion
    float curvature = 0.15;
    vec2 distorted = coord * (1.0 + curvature * dist * dist);
    vec2 curvedUV = distorted + center;
    
    // Clamp to avoid sampling outside texture
    curvedUV = clamp(curvedUV, vec2(0.001), vec2(0.999));
    
    // Sample the screen texture
    vec4 color = texture(screenTexture, curvedUV);
    
    // Scanlines effect
    float scanlineIntensity = 0.3;
    float scanlineFrequency = pc.screenSize.y * 0.5;
    float scanline = sin(curvedUV.y * scanlineFrequency) * 0.5 + 0.5;
    scanline = pow(scanline, 10.0);
    color.rgb *= mix(1.0, scanline, scanlineIntensity);
    
    // Chromatic aberration (RGB separation)
    float chromaAmount = 0.003;
    float chromaDist = dist * chromaAmount;
    float r = texture(screenTexture, curvedUV + vec2(chromaDist, 0.0)).r;
    float g = color.g;
    float b = texture(screenTexture, curvedUV - vec2(chromaDist, 0.0)).b;
    color.rgb = mix(color.rgb, vec3(r, g, b), 0.5);
    
    // Vignette effect
    float vignette = 1.0 - smoothstep(0.4, 1.2, dist);
    color.rgb *= vignette;
    
    // Phosphor glow (slight blur simulation)
    vec4 glow = texture(screenTexture, curvedUV + vec2(0.001, 0.0));
    glow += texture(screenTexture, curvedUV - vec2(0.001, 0.0));
    glow += texture(screenTexture, curvedUV + vec2(0.0, 0.001));
    glow += texture(screenTexture, curvedUV - vec2(0.0, 0.001));
    glow *= 0.25;
    color.rgb = mix(color.rgb, glow.rgb, 0.1);
    
    // Slight brightness boost
    color.rgb *= 1.1;
    
    outColor = color;
}
