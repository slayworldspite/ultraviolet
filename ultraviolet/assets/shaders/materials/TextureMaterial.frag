#version 460 core

layout (location = 0) out vec4 g_Diffuse;
layout (location = 1) out vec4 g_SpecularShininess;
layout (location = 2) out vec4 g_Normal;
layout (location = 3) out vec4 g_Position;

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

in vec2 o_TextureCoordinates;
in mat3 o_TBN;
in vec3 o_FragmentPosition;

uniform TextureMaterial u_TextureMaterial;

void main()
{	
	vec2 textureCoordinates = o_TextureCoordinates * u_TextureMaterial.TextureScale;

	vec4 diffuse = texture(u_TextureMaterial.Diffuse, textureCoordinates);
	if (diffuse.a < 0.1)
		discard;

	float specular = texture(u_TextureMaterial.Specular, textureCoordinates).r;

	vec3 normal = texture(u_TextureMaterial.Normal, textureCoordinates).rgb;

	normal = normal * 2.0 - 1.0;

	if (u_TextureMaterial.FlipNormals)
		normal.y = -normal.y;

	normal.xy *= u_TextureMaterial.NormalStrength;

	normal = normalize(o_TBN * normal);

	g_Diffuse = diffuse;
	g_SpecularShininess = vec4(specular.r, float(u_TextureMaterial.Shininess), 0.0, 1.0);
	g_Normal = vec4(normal, 1.0);
	g_Position = vec4(o_FragmentPosition, 1.0);
}