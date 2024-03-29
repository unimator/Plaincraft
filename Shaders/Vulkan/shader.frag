#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTextCoord;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() {

    outColor = vec4(fragColor, 1.0) * texture(texSampler, fragTextCoord);// * */vec4(fragColor, 1.0);
}
