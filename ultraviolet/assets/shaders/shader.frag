#version 460 core

in vec3 o_Normals;
in vec2 o_TextureCoordinates;

out vec4 Output;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform bool u_HasTexture;

void main()
{
	if (u_HasTexture)
		Output = texture(u_Texture, o_TextureCoordinates);
	else
		Output = u_Color;
}