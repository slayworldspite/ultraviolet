#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TextureCoordinates;
layout (location = 3) in vec3 a_Tangent;

layout (std140, binding = 0) uniform CameraData
{
	mat4 Projection;
	mat4 View;
};

uniform mat4 u_Model;

out vec3 o_Normal;
out vec2 o_TextureCoordinates;
out vec3 o_FragmentPosition;

void main()
{
	gl_Position = Projection * View * u_Model * vec4(a_Position, 1.0);
	
	o_Normal = mat3(u_Model) * a_Normal;
	o_TextureCoordinates = a_TextureCoordinates;
	o_FragmentPosition = vec3(u_Model * vec4(a_Position, 1.0));
}