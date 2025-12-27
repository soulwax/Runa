#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    vec2 screenSize;
    float time;
} pc;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);

    // Time of day cycle (0 = midnight, 0.5 = noon, 1 = midnight again)
    float timeOfDay = mod(pc.time * 0.1, 1.0);

    // Calculate lighting based on time of day
    vec3 nightColor = vec3(0.2, 0.3, 0.6);    // Blue night tint
    vec3 dawnColor = vec3(1.0, 0.7, 0.5);     // Orange dawn
    vec3 dayColor = vec3(1.0, 1.0, 0.95);     // Bright day
    vec3 duskColor = vec3(0.9, 0.5, 0.4);     // Red dusk

    vec3 lightColor;
    float brightness;

    if (timeOfDay < 0.25) {
        // Night to dawn (0.0 - 0.25)
        float t = timeOfDay * 4.0;
        lightColor = mix(nightColor, dawnColor, t);
        brightness = mix(0.3, 0.8, t);
    } else if (timeOfDay < 0.5) {
        // Dawn to day (0.25 - 0.5)
        float t = (timeOfDay - 0.25) * 4.0;
        lightColor = mix(dawnColor, dayColor, t);
        brightness = mix(0.8, 1.0, t);
    } else if (timeOfDay < 0.75) {
        // Day to dusk (0.5 - 0.75)
        float t = (timeOfDay - 0.5) * 4.0;
        lightColor = mix(dayColor, duskColor, t);
        brightness = mix(1.0, 0.7, t);
    } else {
        // Dusk to night (0.75 - 1.0)
        float t = (timeOfDay - 0.75) * 4.0;
        lightColor = mix(duskColor, nightColor, t);
        brightness = mix(0.7, 0.3, t);
    }

    // Apply lighting
    vec3 finalColor = texColor.rgb * lightColor * brightness;

    outColor = vec4(finalColor, texColor.a);
}
