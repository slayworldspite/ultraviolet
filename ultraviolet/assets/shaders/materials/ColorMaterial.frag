#version 460 core

struct ColorMaterial
{
	vec3 Color;
	int Specular;
};

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;

void main()
{
	Output = vec4(u_ColorMaterial.Color, 1.0);
}