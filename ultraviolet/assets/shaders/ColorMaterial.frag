#version 460 core

struct ColorMaterial
{
	vec3 Color;
	float Metallic;
	float Roughness;
};

layout (std140, binding = 1) uniform AmbientLight
{
	vec3 AmbientLightColor;
	float AmbientLightIntensity;
};

in vec3 o_Normal;
in vec3 o_FragmentPosition;

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;

void main()
{
	vec3 diffuse = u_ColorMaterial.Color;

	diffuse *= AmbientLightColor * AmbientLightIntensity;

	Output.rgb = diffuse;

	Output.rgb = u_ColorMaterial.Color;
}