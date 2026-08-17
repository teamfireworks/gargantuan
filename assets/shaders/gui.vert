#version 450

layout(location = 0) in vec2 VertexAbsolutePosition;
layout(location = 1) in vec2 VertexAbsoluteSize;
layout(location = 2) in vec2 VertexUV;
layout(location = 3) in vec4 VertexColor;
layout(location = 4) in int VertexTextureIndex;

layout(location = 0) out vec2 FragmentUV;
layout(location = 1) out vec4 FragmentColor;
layout(location = 2) flat out int FragmentTextureIndex;

layout(set = 1, binding = 0) uniform GuiUniforms {
    vec2 ViewportSize;
} uniforms;

void main() {
    vec2 ndcPos = (VertexAbsolutePosition / uniforms.ViewportSize) * 2.0 - 1.0;
    ndcPos.y = -ndcPos.y;

    gl_Position = vec4(ndcPos, 0.0, 1.0);
    FragmentUV = VertexUV;
    FragmentColor = VertexColor;
    FragmentTextureIndex = VertexTextureIndex;
}
