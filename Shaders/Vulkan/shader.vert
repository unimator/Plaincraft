#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 textMapping;

layout(binding = 0) uniform ModelViewProjectionMatrix {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(inPosition.xy, 0.0, 1.0);
    fragColor = inColor;
}