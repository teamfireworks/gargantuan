#version 450

layout(location = 0) in vec3 VertexPosition;
layout(set = 1, binding = 0) uniform Uniforms {
    mat4 ShadowMatrix;
    mat4 PartMatrix;
} uniforms;

void main()
{
    gl_Position = uniforms.ShadowMatrix * uniforms.PartMatrix * vec4(VertexPosition, 1.0);
}
