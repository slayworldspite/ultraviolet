#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TextureCoordinates;

out vec2 o_TextureCoordinates;

void main()
{
	o_TextureCoordinates = a_TextureCoordinates;
	gl_Position = vec4(a_Position, 1.0);
}