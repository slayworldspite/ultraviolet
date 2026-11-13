	#version 460 core

	out vec4 Output;

	struct AmbientLight
	{
		vec3 Color;
		float Intensity;
	};

	struct DirectionalLight
	{
		vec4 Color;
		vec4 Direction;
		float Intensity;
		bool Enabled;
	};

	layout (std140, binding = 1) uniform AmbientLightData
	{
		AmbientLight ub_AmbientLight;
	};

	layout (std140, binding = 2) uniform DirectionalLightData
	{
		DirectionalLight ub_DirectionalLight;
	};

	in vec2 o_TextureCoordinates;
	in vec3 o_CameraPosition;

	uniform sampler2D u_Diffuse;
	uniform sampler2D u_SpecularShininess;
	uniform sampler2D u_Normal;
	uniform sampler2D u_Position;

	void main()
	{	
		vec3 diffuse = texture(u_Diffuse, o_TextureCoordinates).rgb;

		vec4 specularShininess = texture(u_SpecularShininess, o_TextureCoordinates);
		float specular = specularShininess.r;
		float shininess = specularShininess.g;

		vec3 normal = texture(u_Normal, o_TextureCoordinates).rgb;

		vec3 position = texture(u_Position, o_TextureCoordinates).rgb;

		vec3 viewDirection = normalize(o_CameraPosition - position);

		// shading

		vec3 ambient = diffuse * ub_AmbientLight.Color * ub_AmbientLight.Intensity;

		Output.a = 1.0;
		Output.rgb = ambient;

		if (ub_DirectionalLight.Enabled)
		{
			vec3 lightDirection = normalize(-ub_DirectionalLight.Direction.xyz);

			float diffuseStrength = max(dot(normal, lightDirection), 0.0);
			Output.rgb += diffuse * ub_DirectionalLight.Color.rgb * diffuseStrength * ub_DirectionalLight.Intensity;

			vec3 halfwayDirection = normalize(lightDirection + viewDirection);
			float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), shininess) * specular;
			Output.rgb += specularStrength * ub_DirectionalLight.Color.rgb * 0.25 * ub_DirectionalLight.Intensity;
		}
	}