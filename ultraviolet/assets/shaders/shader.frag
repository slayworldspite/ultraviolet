#version 460 core

struct ColorMaterial
{
	vec4 Color;
	int Specular;
};

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
	float LinearAttenuation;
	float QuadraticAttenuation;
	bool Enabled;
};

in vec3 o_Normals;
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;
uniform sampler2D u_Texture;
uniform bool u_HasTexture;
uniform vec3 u_CameraPosition;
uniform AmbientLight u_AmbientLight;
uniform PointLight[8] u_PointLights;

void main()
{
	vec4 baseColor;

	if (u_HasTexture)
		baseColor = texture(u_Texture, o_TextureCoordinates);
	else
		baseColor = u_ColorMaterial.Color;

	Output.rgb = baseColor.rgb * (u_AmbientLight.Color * u_AmbientLight.Intensity);
	Output.a = baseColor.a;
	
	
	vec3 normal = normalize(o_Normals);
	vec3 viewDirection = normalize(u_CameraPosition - o_FragmentPosition);

	for (int i = 0; i < 8; i++)
	{
		if (!u_PointLights[i].Enabled)
			continue;

		vec3 lightDirection = normalize(u_PointLights[i].Position - o_FragmentPosition);
		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float diffuse =	max(dot(normal, lightDirection), 0.0);
		vec3 specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32) * u_PointLights[i].Color * 0.25;	

		float dist = length(u_PointLights[i].Position - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + u_PointLights[i].LinearAttenuation * dist + u_PointLights[i].QuadraticAttenuation * (dist * dist));

		Output.rgb += baseColor.rgb * u_PointLights[i].Color * diffuse * attenuation * u_PointLights[i].Intensity + specular;
	}
}