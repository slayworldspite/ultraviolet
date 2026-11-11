	#version 460 core

	out vec4 Output;

	in vec2 o_TextureCoordinates;

	uniform sampler2D u_Framebuffer;

	void main()
	{	
		vec4 color = texture(u_Framebuffer, o_TextureCoordinates);

		Output = color;
	}