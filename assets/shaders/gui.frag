#version 450

layout(location = 0) in vec2 FragmentUV;
layout(location = 1) in vec4 FragmentColor;
layout(location = 2) flat in int FragmentTextureIndex;

layout(location = 0) out vec4 OutputColor;

layout(set = 1, binding = 0) uniform GuiUniforms {
    vec2 ViewportSize;
} uniforms;

layout(set = 2, binding = 0) uniform sampler2D UITexture;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    if (FragmentTextureIndex == -1) {
        // Color
        OutputColor = FragmentColor;
    } else if (FragmentTextureIndex == -2) {
        // Font
        vec3 msd = texture(UITexture, FragmentUV).rgb;
        float sd = median(msd.r, msd.g, msd.b);

        vec2 unitRange = vec2(4.0) / vec2(textureSize(UITexture, 0));
        vec2 screenTexSize = vec2(1.0) / fwidth(FragmentUV);
        float screenPxRange = max(0.5 * dot(unitRange, screenTexSize), 1.0);

        float opacity = clamp((sd - 0.5) * screenPxRange + 0.5, 0.0, 1.0);
        OutputColor = vec4(FragmentColor.rgb, FragmentColor.a * opacity);
    } else {
        // Fallback
        OutputColor = FragmentColor;
    }
}
