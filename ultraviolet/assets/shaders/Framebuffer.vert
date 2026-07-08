#version 460 core
layout (location = 0) in vec2 a_Position;
layout (location = 2) in vec2 a_TextureCoordinates;

out vec2 o_TextureCoordinates;

void main()
{
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0); 
    o_TextureCoordinates = a_TextureCoordinates;
} 