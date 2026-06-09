#version 460 core

out vec4 Output;

in vec3 o_Color;

void main()
{
	Output = vec4(o_Color, 1.0);
}