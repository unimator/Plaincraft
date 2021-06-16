#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 textCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
out vec2 TextCoord;

void main() {
	gl_Position = projection * view * model * vec4(position.xyz, 1.0);
	TextCoord = textCoord;
	Color = color;
}