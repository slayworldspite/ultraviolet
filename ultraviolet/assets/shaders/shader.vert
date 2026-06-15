#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normals;
layout (location = 2) in vec2 a_TextureCoordinates;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec3 o_Normals;
out vec2 o_TextureCoordinates;
out vec3 o_FragmentPosition;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
	
	o_Normals = a_Normals;
	o_TextureCoordinates = a_TextureCoordinates;
	o_FragmentPosition = vec3(u_Model * vec4(a_Position, 1.0));
}