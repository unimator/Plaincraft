#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 textMapping;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTextCoord;

layout(binding = 0) uniform ModelViewProjectionMatrix {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 color;
} mvp;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 3.0, -1.0));
const float AMBIENT = 0.05;

void main() {
    gl_Position = mvp.projection * mvp.view * mvp.model * vec4(inPosition, 1.0);
    
    vec3 normalWorldSpace = normalize(mat3(mvp.model) * normal);
    float lightIntensity = max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);
    lightIntensity = max(lightIntensity, AMBIENT);

    fragColor = mvp.color * lightIntensity;
    fragTextCoord = textMapping;
}