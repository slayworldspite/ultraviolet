	#version 460 core

	struct TextureMaterial
	{
		sampler2D Diffuse;
		sampler2D Specular;
		sampler2D Normal;
		int Shininess;
		float TextureScale;
		float NormalStrength;
		bool FlipNormals;
	};

	in vec2 o_TextureCoordinates;

	out vec4 Output;

	uniform TextureMaterial u_TextureMaterial;

	void main()
	{	
		vec4 diffuse = texture(u_TextureMaterial.Diffuse, o_TextureCoordinates * u_TextureMaterial.TextureScale);
		if (diffuse.a < 0.1)
			discard;

		Output = diffuse;
	}