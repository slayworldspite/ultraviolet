#version 460 core

out vec4 Output;
  
in vec2 o_TextureCoordinates;

uniform sampler2D u_Framebuffer;
uniform vec2 u_Resolution;
uniform vec2 u_Direction;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur
vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
    vec4 color = vec4(0.0);
    vec2 off1 = vec2(2.5) * direction;
    vec2 off2 = vec2(5.5) * direction;
    color += texture2D(image, uv) * 0.2270270270;
    color += texture2D(image, uv + (off1 / resolution)) * 0.3162162162;
    color += texture2D(image, uv - (off1 / resolution)) * 0.3162162162;
    color += texture2D(image, uv + (off2 / resolution)) * 0.0702702703;
    color += texture2D(image, uv - (off2 / resolution)) * 0.0702702703;
    return color;
}

void main()
{
    Output = blur9(u_Framebuffer, o_TextureCoordinates, u_Resolution, u_Direction);
}