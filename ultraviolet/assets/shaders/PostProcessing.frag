#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;
uniform sampler2D u_Blur;
uniform sampler2D u_Overlay;
uniform sampler2D u_NormalMap;
uniform sampler2D u_SSAO;

uniform bool u_EnableBloom;
uniform bool u_EnableSSAO;
uniform float u_BloomStrength;
uniform float u_SSAOStrength;

void main()
{
    vec4 color = texture(u_Framebuffer, o_TextureCoordinates);

    if (u_EnableBloom)
        color.rgb += texture(u_Blur, o_TextureCoordinates).rgb * u_BloomStrength;
    
    float ao = texture(u_SSAO, o_TextureCoordinates).r;
    
    if (u_EnableSSAO)
        color.rgb *= mix(1.0, ao, u_SSAOStrength);

    Output.rgb = pow(color.rgb, vec3(1.0 / 2.2));

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