#version 330 core
out vec4 FragColor;

in vec3 color;

uniform sampler2D texture1;

void main()
{
	FragColor = vec4(color, 1);
}