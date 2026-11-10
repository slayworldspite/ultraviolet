#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TextureCoordinates;

layout (std140, binding = 0) uniform CameraData
{
	mat4 ub_Projection;
	mat4 ub_View;
	vec3 ub_CameraPosition;
};

out vec2 o_TextureCoordinates;
out vec3 o_CameraPosition;

void main()
{
	o_TextureCoordinates = a_TextureCoordinates;
	o_CameraPosition = ub_CameraPosition;
	gl_Position = vec4(a_Position, 1.0);
}