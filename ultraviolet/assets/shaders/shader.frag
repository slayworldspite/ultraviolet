#version 460 core

const float PI = 3.14159265359;

struct ColorMaterial
{
	vec3 Color;
	float Metallic;
	float Roughness;
};

struct TextureMaterial
{
	sampler2D Albedo;
	sampler2D Metallic;
	sampler2D Roughness;
	sampler2D Normal;
	sampler2D AmbientOcclusion;

	bool HasAlbedo;
	bool HasMetallic;
	bool HasRoughness;
	bool HasNormal;
	bool HasAmbientOcclusion;
};

struct AmbientLight
{
	vec3 Color;
	float Intensity;
	bool Enabled;
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

struct DirectionalLight
{
	vec3 Color;
	vec3 Direction;
	float Intensity;
	bool Enabled;
};

in vec3 o_Normal;
in vec2 o_TextureCoordinates;
in vec3 o_FragmentPosition;
in vec4 o_FragmentPositionLightSpace;

out vec4 Output;

uniform ColorMaterial u_ColorMaterial;
uniform TextureMaterial u_TextureMaterial;
uniform DirectionalLight u_DirectionalLight;
uniform bool u_HasTexture;
uniform vec3 u_CameraPosition;
uniform AmbientLight u_AmbientLight;
uniform PointLight u_PointLights[8];
uniform mat4 u_LightSpaceMatrix;
uniform sampler2D u_ShadowMap;

vec3 GetNormal(vec3 normal);
float DistributionGGX(vec3 n, vec3 h, float roughness);
float GeometrySchlickGGX(float nDotV, float roughness);
float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 f0);
float CalculateShadow();

void main()
{
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
	float ambientOcclusion;
	
	if (u_HasTexture) 
	{
		albedo = texture(u_TextureMaterial.Albedo, o_TextureCoordinates).rgb;
		normal = texture(u_TextureMaterial.Normal, o_TextureCoordinates).rgb;
		metallic = texture(u_TextureMaterial.Metallic, o_TextureCoordinates).r;
		roughness = texture(u_TextureMaterial.Roughness, o_TextureCoordinates).r;
		ambientOcclusion = texture(u_TextureMaterial.AmbientOcclusion, o_TextureCoordinates).r;
	}
	else
	{
		albedo = u_ColorMaterial.Color.rgb;
		normal = o_Normal;
		metallic = u_ColorMaterial.Metallic;
		roughness = u_ColorMaterial.Roughness;
		ambientOcclusion = 1.0;
	}

	normal = GetNormal(normal);

	normal.xy *= 1.5;
	normal = normalize(normal);

	vec3 n = normalize(normal);
	vec3 v = normalize(u_CameraPosition - o_FragmentPosition);

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	vec3 lo = vec3(0.0);

	if (u_DirectionalLight.Enabled)
	{
		vec3 l = normalize(-u_DirectionalLight.Direction);
		vec3 h = normalize(v + l);

		vec3 radiance = u_DirectionalLight.Color * u_DirectionalLight.Intensity;

		float ndf = DistributionGGX(n, h, roughness);
		float g = GeometrySmith(n, v, l, roughness);
		vec3 f = FresnelSchlick(max(dot(h, v), 0.0), f0);

		vec3 kS = f;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = ndf * g * f;
		float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float nDotL = max(dot(n, l), 0.0);

		lo += (kD * albedo / PI + specular) * radiance * nDotL;
	}

	float shadow = CalculateShadow();
	lo *= 1.25 - shadow;

	for (int i = 0; i < 8; i++)
	{
		if (!u_PointLights[i].Enabled)
			continue;

		vec3 l = normalize(u_PointLights[i].Position - o_FragmentPosition);
		vec3 h = normalize(v + l);

		float dist = length(u_PointLights[i].Position - o_FragmentPosition);
		float attenuation = 1.0 / (1.0 + u_PointLights[i].LinearAttenuation * dist + u_PointLights[i].QuadraticAttenuation * (dist * dist));
		vec3 radiance = u_PointLights[i].Color * attenuation * u_PointLights[i].Intensity;

		float ndf = DistributionGGX(n, h, roughness);
		float g = GeometrySmith(n, v, l, roughness);
		vec3 f = FresnelSchlick(max(dot(h, v), 0.0), f0);

		vec3 kS = f;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = ndf * g * f;
		float denominator = 4.0 * max(dot(n, v), 0.0) *  max(dot(n, l), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float nDotL = max(dot(n, l), 0.0);
		lo += (kD * albedo / PI + specular) * radiance * nDotL;
	}

	vec3 ambient = vec3(0.0);

	if (u_AmbientLight.Enabled)
		ambient = albedo * u_AmbientLight.Color * u_AmbientLight.Intensity;

	vec3 color = ambient + lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	Output = vec4(color, 1.0);
}

// https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.2.lighting_textured/1.2.pbr.fs
vec3 GetNormal(vec3 normal)
{
    vec3 tangentNormal = normal * 2.0 - 1.0;

    vec3 Q1  = dFdx(o_FragmentPosition);
    vec3 Q2  = dFdy(o_FragmentPosition);
    vec2 st1 = dFdx(o_TextureCoordinates);
    vec2 st2 = dFdy(o_TextureCoordinates);

    vec3 N   = normalize(o_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 n, vec3 h, float roughness) 
{
	float a = roughness * roughness;
	float a2 = a * a;
	float nDotH = max(dot(n, h), 0.0);
	float nDotH2 = nDotH * nDotH;

	float numerator = a2;
	float denominator = (nDotH2 * (a2 - 1.0) + 1.0);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float nDotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float numerator = nDotV;
	float denominator = nDotV * (1.0 - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float nDotV = max(dot(n, v), 0.0);
    float nDotL = max(dot(n, l), 0.0);
    float ggx2  = GeometrySchlickGGX(nDotV, roughness);
    float ggx1  = GeometrySchlickGGX(nDotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float CalculateShadow()
{
    vec3 projectionCoordinates = o_FragmentPositionLightSpace.xyz / o_FragmentPositionLightSpace.w;

    projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

    if (projectionCoordinates.z > 1.0)
        return 0.0;
	
    float closestDepth = texture(u_ShadowMap, projectionCoordinates.xy).r;
	float currentDepth = projectionCoordinates.z;

    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);

    float shadow = 0.0;

    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            float closestDepth = texture(
                u_ShadowMap,
                projectionCoordinates.xy + vec2(x, y) * texelSize
            ).r;

            float bias = 0.005;

            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 25.0;

    return shadow;
}