#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

out vec3 o_Color;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	o_Color = a_Color;
}