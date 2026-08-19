#version 460 core

struct LightMaterial
{
	vec3 Color;
	float Intensity;
	bool Enabled;
};

out vec4 Output;

uniform LightMaterial u_LightMaterial;

void main()
{
	Output.a = 1.0;

	if (u_LightMaterial.Enabled)
		Output.rgb = (vec3(1.0) + u_LightMaterial.Color) * u_LightMaterial.Intensity;
}