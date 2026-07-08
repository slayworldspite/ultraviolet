#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;
uniform sampler2D u_Blur;
uniform sampler2D u_Blur1;

uniform bool u_EnableBloom;

void main()
{
    vec4 color = texture(u_Framebuffer, o_TextureCoordinates);

    if (u_EnableBloom)
    {
        color.rgb += texture(u_Blur, o_TextureCoordinates).rgb * 0.5;
        //color.rgb += texture(u_Blur1, o_TextureCoordinates).rgb * 0.25;
    }

    Output.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    Output.a = 1.0;
}