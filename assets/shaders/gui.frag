#version 450

layout(location = 0) in vec2 FragmentUV;
layout(location = 1) in vec4 FragmentColor;
layout(location = 2) flat in int FragmentTextureIndex;

layout(location = 0) out vec4 OutputColor;

layout(set = 1, binding = 0) uniform GuiUniforms {
    vec2 ViewportSize;
} uniforms;

layout(set = 2, binding = 0) uniform sampler2D FontTexture;

void main() {
    if (FragmentTextureIndex == -2) {
        OutputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    } else {
        OutputColor = FragmentColor;
    }
}
