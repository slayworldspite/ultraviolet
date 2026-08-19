#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;
uniform sampler2D u_Overlay;
uniform float u_Gamma;

void main()
{
    vec4 color = texture(u_Framebuffer, o_TextureCoordinates);

    Output.rgb = pow(color.rgb, vec3(1.0 / u_Gamma));

    vec4 overlay = texture(u_Overlay, o_TextureCoordinates);

    if (overlay.a > 0.0)
    {
        vec3 inverted = vec3(1.0) - Output.rgb;
        Output.rgb = mix(Output.rgb, inverted, overlay.a);
        
        if (overlay.r > 0.25 && overlay.g < 0.01 && overlay.b < 0.01)
            Output.rgb = vec3(1.0, 0.0, 0.0);
    } 

    Output.a = 1.0;
}