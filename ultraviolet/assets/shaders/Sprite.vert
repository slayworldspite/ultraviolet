#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TextureCoordinates;

uniform mat4 u_Model;
uniform mat4 u_Projection;

out vec2 o_TextureCoordinates;

void main() 
{
	gl_Position = u_Projection * u_Model * vec4(a_Position, 0.0f, 1.0f);
	o_TextureCoordinates = a_TextureCoordinates; 
}