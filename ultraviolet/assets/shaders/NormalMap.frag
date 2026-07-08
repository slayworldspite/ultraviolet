#version 460 core

in vec3 o_Normal;

out vec4 Output;

void main()
{        
	vec3 normal = normalize(o_Normal) * 0.5 + 0.5;

	Output = vec4(normal, 1.0);
}  