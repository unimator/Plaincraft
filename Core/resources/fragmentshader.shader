#version 330

out vec4 color;

in vec2 TextCoord;
in vec3 Color;

uniform sampler2D texture1;

void main() {
	color = texture(texture1, TextCoord) * vec4(Color, 1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}