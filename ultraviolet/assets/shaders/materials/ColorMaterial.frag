#version 460 core

#define POINT_LIGHT_MAX_COUNT 16

struct ColorMaterial
{
	vec3 Color;
	int Specular;
};

struct AmbientLight
{
	vec3 Color;
	float Intensity;
};

struct DirectionalLight
{
	vec4 Color;
	vec4 Direction;
	float Intensity;
	bool Enabled;
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

layout (std140, binding = 2) uniform DirectionalLightData
{
	DirectionalLight ub_DirectionalLight;
};

layout (std140, binding = 3) uniform PointLightsData
{
	PointLight ub_PointLights[POINT_LIGHT_MAX_COUNT];
	int ub_MaxPointLightIndex;
};

in vec3 o_Normal;
in vec3 o_FragmentPosition;
in vec4 o_FragmentPositionLightSpace;
in vec3 o_CameraPosition;

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;
uniform sampler2D u_DepthMap;

void main()
{
	vec3 viewDirection = normalize(o_CameraPosition - o_FragmentPosition);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 ambient = u_ColorMaterial.Color * ub_AmbientLight.Color * ub_AmbientLight.Intensity;

	if (ub_DirectionalLight.Enabled)
	{
		vec3 lightDirection = normalize(-ub_DirectionalLight.Direction.xyz);

		float diffuseStrength = max(dot(o_Normal, lightDirection), 0.0);
		diffuse += u_ColorMaterial.Color * ub_DirectionalLight.Color.rgb * diffuseStrength * ub_DirectionalLight.Intensity;

		vec3 reflectionDirection = reflect(-lightDirection, o_Normal);

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularStrength = pow(max(dot(o_Normal, halfwayDirection), 0.0), u_ColorMaterial.Specular);
		specular += specularStrength * ub_DirectionalLight.Color.rgb * 0.25 * ub_DirectionalLight.Intensity;
	
		vec3 projectionCoordinates = o_FragmentPositionLightSpace.xyz / o_FragmentPositionLightSpace.w;
		projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

		float currentDepth = projectionCoordinates.z;
		float shadow = 0.0;

		vec2 texelSize = 1.0 / textureSize(u_DepthMap, 0);

		int radius = 2;

		for (int x = 0 - radius; x <= radius; x++)
		{
			for (int y = 0 - radius; y <= radius; y++)
			{
				float pcfDepth = texture(u_DepthMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth > pcfDepth ? 1.0 : 0.0;    
			}    
		}

		int radiusWidth = 2 * radius + 1;
		shadow /= radiusWidth * radiusWidth;

		if (projectionCoordinates.z > 1.0)
			shadow = 0.0;

		Output.rgb = ambient + (1.0 - shadow) * (diffuse + specular);
	}

	diffuse = vec3(0.0);
	specular = vec3(0.0);

	for (int i = 0; i < ub_MaxPointLightIndex; i++)
	{
		if (!ub_PointLights[i].Enabled)
			continue;
		
		float dist = length(ub_PointLights[i].Position.xyz - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + ub_PointLights[i].LinearAttenuation * dist + ub_PointLights[i].QuadraticAttenuation * (dist * dist));

		vec3 lightDirection = normalize(ub_PointLights[i].Position.xyz - o_FragmentPosition);

		float diffuseStrength = max(dot(o_Normal, lightDirection), 0.0);
		diffuse += ub_PointLights[i].Color.rgb * diffuseStrength * ub_PointLights[i].Intensity * attenuation * u_ColorMaterial.Color;

		vec3 reflectionDirection = reflect(-lightDirection, o_Normal);

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularStrength = pow(max(dot(o_Normal, halfwayDirection), 0.0), u_ColorMaterial.Specular);
		specular += specularStrength * ub_PointLights[i].Color.rgb * 0.25 * ub_PointLights[i].Intensity * attenuation;
	}

	Output.rgb += diffuse + specular;

	Output.a = 1.0;
}