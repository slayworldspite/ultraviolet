#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Overlay;

void main()
{
    Output = texture(u_Overlay, o_TextureCoordinates);
}