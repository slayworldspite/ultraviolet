#version 460 core

struct AmbientLight
{
	vec3 Color;
	float Intensity;
};

struct PointLight
{
	vec3 Position;
	vec3 Color;
	float Intensity;
	bool Enabled;
};

in vec3 o_Normals;
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;

out vec4 Output;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform bool u_HasTexture;
uniform AmbientLight u_AmbientLight;
uniform PointLight[8] u_PointLights;

void main()
{
	vec3 normal = normalize(o_Normals);

	vec4 baseColor;

	if (u_HasTexture)
		baseColor = texture(u_Texture, o_TextureCoordinates);
	else
		baseColor = u_Color;

	Output.rgb = baseColor.rgb * (u_AmbientLight.Color * u_AmbientLight.Intensity);
	Output.a = baseColor.a;
	
	for (int i = 0; i < 8; i++)
	{
		if (!u_PointLights[i].Enabled)
			continue;

		vec3 lightDirection = normalize(u_PointLights[i].Position - o_FragmentPosition);
		float diffuse =	max(dot(normal, lightDirection), 0.0);

		float distance = length(u_PointLights[i].Position - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

		Output.rgb += baseColor.rgb * u_PointLights[i].Color * diffuse * attenuation * u_PointLights[i].Intensity;
	}
}