#version 460 core

#define POINT_LIGHT_MAX_COUNT 16

struct TextureMaterial
{
	sampler2D Diffuse;
	sampler2D Specular;
	int Shininess;
};

struct AmbientLight
{
	vec3 Color;
	float Intensity;
};

struct PointLight
{
	vec4 Color;
	vec4 Position;
	float Intensity;
	float LinearAttenuation;
	float QuadraticAttenuation;
	bool Enabled;
};

layout (std140, binding = 1) uniform AmbientLightData
{
	AmbientLight ub_AmbientLight;
};

layout (std140, binding = 2) uniform PointLights
{
	PointLight ub_PointLights[POINT_LIGHT_MAX_COUNT];
	int ub_MaxPointLightIndex;
};

in vec3 o_Normal;
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;
in vec3 o_CameraPosition;

out vec4 Output;

uniform TextureMaterial u_TextureMaterial;

void main()
{
	vec3 normal = normalize(o_Normal);

	if (gl_FrontFacing)
		normal = -normal;

	vec3 viewDirection = normalize(o_CameraPosition - o_FragmentPosition);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 ambient = ub_AmbientLight.Color * ub_AmbientLight.Intensity;

	for (int i = 0; i < ub_MaxPointLightIndex; i++)
	{
		if (!ub_PointLights[i].Enabled)
			continue;
		
		float dist = length(ub_PointLights[i].Position.xyz - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + ub_PointLights[i].LinearAttenuation * dist + ub_PointLights[i].QuadraticAttenuation * (dist * dist));

		vec3 lightDirection = normalize(ub_PointLights[i].Position.xyz - o_FragmentPosition);

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		diffuse += ub_PointLights[i].Color.rgb * diffuseStrength * ub_PointLights[i].Intensity * attenuation;

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float specularStrength = pow(max(dot(viewDirection, reflectionDirection), 0.0), u_TextureMaterial.Shininess);
		specular += specularStrength * ub_PointLights[i].Color.rgb * 0.25 * ub_PointLights[i].Intensity * attenuation * texture(u_TextureMaterial.Specular, o_TextureCoordinates).rgb;
	}

	Output.rgb = (diffuse + ambient + specular) * texture(u_TextureMaterial.Diffuse, o_TextureCoordinates).rgb;
	Output.a = 1.0;
}