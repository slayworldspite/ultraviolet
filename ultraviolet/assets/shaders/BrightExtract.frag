#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;

void main()
{
    Output = texture(u_Framebuffer, o_TextureCoordinates);

    float brightness = dot(Output.rgb, vec3(0.2126, 0.7152, 0.0722));

    float factor = smoothstep(0.4, 0.6, brightness);

    Output.rgb *= factor;
}