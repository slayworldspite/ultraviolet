#version 460 core

#define POINT_LIGHT_MAX_COUNT 16

struct TextureMaterial
{
	sampler2D Diffuse;
	sampler2D Specular;
	sampler2D Normal;
	int Shininess;
	float TextureScale;
	float NormalStrength;
	bool FlipNormals;
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
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;
in vec4 o_FragmentPositionLightSpace;
in vec3 o_CameraPosition;
in mat3 o_TBN;

out vec4 Output;

uniform TextureMaterial u_TextureMaterial;
uniform sampler2D u_DepthMap;

void main()
{
	vec2 textureCoordinates = o_TextureCoordinates * u_TextureMaterial.TextureScale;

	vec3 normal = texture(u_TextureMaterial.Normal, textureCoordinates).rgb;

	normal = normal * 2.0 - 1.0;

	if (u_TextureMaterial.FlipNormals)
		normal.y = -normal.y;

	normal.xy *= u_TextureMaterial.NormalStrength;
	
	normal = normalize(o_TBN * normal);

	vec3 viewDirection = normalize(o_CameraPosition - o_FragmentPosition);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 ambient = texture(u_TextureMaterial.Diffuse, textureCoordinates).rgb * ub_AmbientLight.Color * ub_AmbientLight.Intensity;

	if (ub_DirectionalLight.Enabled)
	{
		vec3 lightDirection = normalize(-ub_DirectionalLight.Direction.xyz);

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		diffuse += texture(u_TextureMaterial.Diffuse, textureCoordinates).rgb * ub_DirectionalLight.Color.rgb * diffuseStrength * ub_DirectionalLight.Intensity;

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), u_TextureMaterial.Shininess) * texture(u_TextureMaterial.Specular, textureCoordinates).r;
		specular += specularStrength * ub_DirectionalLight.Color.rgb * 0.25 * ub_DirectionalLight.Intensity;
	}

	vec3 projectionCoordinates = o_FragmentPositionLightSpace.xyz / o_FragmentPositionLightSpace.w;
    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

    float closestDepth = texture(u_DepthMap, projectionCoordinates.xy).r; 

    float currentDepth = projectionCoordinates.z;

	float bias = max(0.0025 * (1.0 - dot(o_Normal, normalize(-ub_DirectionalLight.Direction.xyz))), 0.0025);  
    float shadow = 0.0;

	vec2 texelSize = 1.0 / textureSize(u_DepthMap, 0);

	for (int x = -2; x <= 2; x++)
	{
		for (int y = -2; y <= 2; y++)
		{
			float pcfDepth = texture(u_DepthMap, projectionCoordinates.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}

	shadow /= 25.0;

	if (projectionCoordinates.z > 1.0)
		shadow = 0.0;

	Output.rgb = ambient + (1.0 - shadow) * (diffuse + specular);

	diffuse = vec3(0.0);
	specular = vec3(0.0);

	for (int i = 0; i < ub_MaxPointLightIndex; i++)
	{
		if (!ub_PointLights[i].Enabled)
			continue;
		
		float dist = length(ub_PointLights[i].Position.xyz - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + ub_PointLights[i].LinearAttenuation * dist + ub_PointLights[i].QuadraticAttenuation * (dist * dist));

		vec3 lightDirection = normalize(ub_PointLights[i].Position.xyz - o_FragmentPosition);

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		diffuse += ub_PointLights[i].Color.rgb * diffuseStrength * ub_PointLights[i].Intensity * attenuation * texture(u_TextureMaterial.Diffuse, textureCoordinates).rgb;

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), u_TextureMaterial.Shininess) * texture(u_TextureMaterial.Specular, textureCoordinates).r;
		specular += specularStrength * ub_PointLights[i].Color.rgb * 0.25 * ub_PointLights[i].Intensity * attenuation;
	}

	Output.rgb += diffuse + specular;

	Output.a = 1.0;
}