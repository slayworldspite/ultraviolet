#version 460 core

layout (location = 0) out vec4 g_Diffuse;
layout (location = 1) out vec4 g_SpecularShininess;
layout (location = 2) out vec4 g_Normal;
layout (location = 3) out vec4 g_Position;

struct ColorMaterial
{
	vec3 Color;
	int Specular;
};

in vec3 o_Normal;
in vec3 o_FragmentPosition;

uniform ColorMaterial u_ColorMaterial;

void main()
{
	g_Diffuse = vec4(u_ColorMaterial.Color, 1.0);
	g_SpecularShininess = vec4(1.0, u_ColorMaterial.Specular, 0.0, 1.0);
	g_Normal = vec4(o_Normal, 1.0);
	g_Position = vec4(o_FragmentPosition, 1.0);
}