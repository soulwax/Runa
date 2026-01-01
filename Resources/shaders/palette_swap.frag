# File: Resources/shaders/palette_swap.frag
# version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    vec2 screenSize;
    float time;
} pc;

// Define color palettes (you can customize these)
vec3 originalPalette[4] = vec3[](
    vec3(0.2, 0.1, 0.05),  // Dark brown
    vec3(0.6, 0.3, 0.15),  // Medium brown
    vec3(0.9, 0.6, 0.3),   // Light brown
    vec3(1.0, 0.9, 0.7)    // Beige
);

vec3 swapPalette1[4] = vec3[](
    vec3(0.1, 0.1, 0.3),   // Dark blue
    vec3(0.2, 0.3, 0.7),   // Medium blue
    vec3(0.4, 0.6, 1.0),   // Light blue
    vec3(0.8, 0.9, 1.0)    // Pale blue
);

vec3 swapPalette2[4] = vec3[](
    vec3(0.2, 0.0, 0.1),   // Dark red
    vec3(0.6, 0.1, 0.2),   // Medium red
    vec3(0.9, 0.3, 0.4),   // Light red
    vec3(1.0, 0.7, 0.8)    // Pink
);

// Find closest color in palette
int findClosestColor(vec3 color, vec3 palette[4]) {
    int closest = 0;
    float minDist = distance(color, palette[0]);

    for (int i = 1; i < 4; i++) {
        float dist = distance(color, palette[i]);
        if (dist < minDist) {
            minDist = dist;
            closest = i;
        }
    }

    return closest;
}

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);

    // Use time to cycle between palettes (0 = original, 1-2 = palette1, 2-3 = palette2)
    int paletteIndex = int(mod(pc.time, 3.0));

    vec3 finalColor = texColor.rgb;

    if (paletteIndex > 0) {
        // Find closest color in original palette
        int colorIndex = findClosestColor(texColor.rgb, originalPalette);

        // Swap to new palette
        if (paletteIndex == 1) {
            finalColor = swapPalette1[colorIndex];
        } else if (paletteIndex == 2) {
            finalColor = swapPalette2[colorIndex];
        }
    }

    outColor = vec4(finalColor, texColor.a);
}
