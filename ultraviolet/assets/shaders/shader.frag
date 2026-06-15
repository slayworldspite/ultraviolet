#version 460 core

struct AmbientLight
{
	vec3 Color;
	float Intensity;
};

in vec3 o_Normals;
in vec2 o_TextureCoordinates;

out vec4 Output;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform bool u_HasTexture;
uniform AmbientLight u_AmbientLight;

vec4 AddAmbientLight(vec4 color);

void main()
{
	if (u_HasTexture)
		Output = texture(u_Texture, o_TextureCoordinates);
	else
		Output = u_Color;

	Output = AddAmbientLight(Output);
}

vec4 AddAmbientLight(vec4 color) 
{
	return color * vec4(u_AmbientLight.Color * u_AmbientLight.Intensity, 1.0);
}