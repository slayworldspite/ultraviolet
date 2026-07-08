#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TextureCoordinates;
layout (location = 3) in vec3 a_Tangent;

layout (std140, binding = 0) uniform CameraData
{
	mat4 ub_Projection;
	mat4 ub_View;
	vec3 ub_CameraPosition;
};

layout (std140, binding = 4) uniform DepthMapData
{
	mat4 ub_LightSpaceMatrix;
};

uniform mat4 u_Model;

out vec3 o_Normal;
out vec2 o_TextureCoordinates;
out vec3 o_FragmentPosition;
out vec4 o_FragmentPositionLightSpace;
out vec3 o_CameraPosition;
out mat3 o_TBN;

void main()
{
	gl_Position = ub_Projection * ub_View * u_Model * vec4(a_Position, 1.0);
	
	o_Normal = mat3(u_Model) * a_Normal;
	o_TextureCoordinates = a_TextureCoordinates;
	o_FragmentPosition = vec3(u_Model * vec4(a_Position, 1.0));
	o_FragmentPositionLightSpace = ub_LightSpaceMatrix * vec4(o_FragmentPosition, 1.0);
	o_CameraPosition = ub_CameraPosition;

	vec3 t = normalize(vec3(u_Model * vec4(a_Tangent, 0.0)));
	vec3 n = normalize(vec3(u_Model * vec4(a_Normal, 0.0)));
	vec3 b = cross(n, t);

	o_TBN = mat3(t, b, n);
}