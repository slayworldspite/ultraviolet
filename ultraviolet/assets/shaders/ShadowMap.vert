#version 460 core
layout (location = 0) in vec3 a_Position;

layout (std140, binding = 4) uniform DepthMapData
{
	mat4 ub_LightSpaceMatrix;
};

uniform mat4 u_Model;

void main()
{
    gl_Position = ub_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
}  