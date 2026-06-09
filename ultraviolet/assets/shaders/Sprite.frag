#version 460 core

in vec2 o_TextureCoordinates;

out vec4 o_Output;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform bool u_HasTexture;

void main()
{
	if (u_HasTexture)
		o_Output = texture(u_Texture, o_TextureCoordinates);
	else
		o_Output = vec4(u_Color);
}