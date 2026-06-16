#version 460 core

struct ColorMaterial
{
	vec4 Color;
	int Specular;
};

struct TextureMaterial
{
	sampler2D Diffuse;
	sampler2D Specular;
	sampler2D Normal;
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

in vec3 o_Normal;
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;
in mat3 o_TBN;
in vec3 o_TangentViewPosition;
in vec3 o_TangentFragmentPosition;

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;
uniform TextureMaterial u_TextureMaterial;
uniform bool u_HasTexture;
uniform vec3 u_CameraPosition;
uniform AmbientLight u_AmbientLight;
uniform PointLight u_PointLights[8];

void ShadeColorMaterial(vec4 baseColor);
void ShadeTextureMaterial(vec4 baseColor);

void main()
{
	vec4 baseColor;

	if (u_HasTexture) 
	{
		baseColor = texture(u_TextureMaterial.Diffuse, o_TextureCoordinates);
		ShadeTextureMaterial(baseColor);
	}
	else
	{
		baseColor = u_ColorMaterial.Color;
		ShadeColorMaterial(baseColor);
	}
}

void ShadeColorMaterial(vec4 baseColor)
{
	Output.rgb = baseColor.rgb * (u_AmbientLight.Color * u_AmbientLight.Intensity);
	Output.a = baseColor.a;
	
	vec3 normal = normalize(o_Normal);
	vec3 viewDirection = normalize(u_CameraPosition - o_FragmentPosition);

	for (int i = 0; i < 8; i++)
	{
		if (!u_PointLights[i].Enabled)
			continue;

		vec3 lightDirection = normalize(u_PointLights[i].Position - o_FragmentPosition);

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float diffuse =	max(dot(normal, lightDirection), 0.0);
		vec3 specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), u_ColorMaterial.Specular) * u_PointLights[i].Color * 0.25;	

		float dist = length(u_PointLights[i].Position - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + u_PointLights[i].LinearAttenuation * dist + u_PointLights[i].QuadraticAttenuation * (dist * dist));

		Output.rgb += baseColor.rgb * u_PointLights[i].Color * diffuse * attenuation * u_PointLights[i].Intensity + specular;
	}
}


void ShadeTextureMaterial(vec4 baseColor)
{
	vec3 normal = texture(u_TextureMaterial.Normal, o_TextureCoordinates).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	Output.rgb = baseColor.rgb * (u_AmbientLight.Color * u_AmbientLight.Intensity);
	Output.a = baseColor.a;

	for (int i = 0; i < 8; i++)
	{
		if (!u_PointLights[i].Enabled)
			continue;

		vec3 lightDirection = normalize(o_TBN * u_PointLights[i].Position - o_TangentFragmentPosition);

		float dist = length(u_PointLights[i].Position - o_TangentFragmentPosition);
		float attenuation = 1.0 / (1.0 + u_PointLights[i].LinearAttenuation * dist + u_PointLights[i].QuadraticAttenuation * (dist * dist));

		vec3 diffuse = u_PointLights[i].Color * max(dot(lightDirection, normal), 0.0);

		vec3 viewDirection = normalize(o_TangentViewPosition - o_TangentFragmentPosition);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);

		vec3 specular =	pow(max(dot(normal, halfwayDirection), 0.0), 32) * u_PointLights[i].Color;

		Output.rgb += baseColor.rgb * (diffuse + specular) * attenuation * u_PointLights[i].Intensity;	
	}
}
