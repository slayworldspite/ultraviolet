#version 460 core

out vec4 Output;

uniform vec4 u_Color;

void main()
{
	Output = u_Color;
}