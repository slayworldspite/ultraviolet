#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TextureCoordinates;
layout (location = 3) in vec3 a_Tangent;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform vec3 u_CameraPosition;

out vec3 o_Normal;
out vec2 o_TextureCoordinates;
out vec3 o_FragmentPosition;
out mat3 o_TBN;
out vec3 o_TangentViewPosition;
out vec3 o_TangentFragmentPosition;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
	
	o_Normal = mat3(u_Model) * a_Normal;
	o_TextureCoordinates = a_TextureCoordinates;
	o_FragmentPosition = vec3(u_Model * vec4(a_Position, 1.0));

	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	vec3 t = normalize(normalMatrix * a_Tangent);
	vec3 n = normalize(normalMatrix * a_Normal);
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(n, t);

	o_TBN = transpose(mat3(t, b, n));
	o_TangentViewPosition = o_TBN * u_CameraPosition;
	o_TangentFragmentPosition = o_TBN * o_FragmentPosition;
}