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

uniform mat4 u_Model;

void main()
{
    gl_Position = ub_Projection * ub_View * u_Model * vec4(a_Position, 1.0);
    o_TextureCoordinates = a_TextureCoordinates;
} 