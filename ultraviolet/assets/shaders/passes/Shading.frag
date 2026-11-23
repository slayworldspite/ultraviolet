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

	struct PointLight
	{
		vec4 Color;
		vec4 Position;
		float Intensity;
		float Radius;
		bool Enabled;
	};

	layout (std140, binding = 0) uniform AmbientLightData
	{
		AmbientLight ub_AmbientLight;
	};

	layout (std140, binding = 1) uniform DirectionalLightData
	{
		DirectionalLight ub_DirectionalLight;
	};
	
	layout (std430, binding = 0) buffer PointLightBuffer
	{
		PointLight ub_PointLights[];
	};

	in vec2 o_TextureCoordinates;
	in vec3 o_CameraPosition;

	uniform sampler2D u_Diffuse;
	uniform sampler2D u_SpecularShininess;
	uniform sampler2D u_Normal;
	uniform sampler2D u_Position;
	uniform int u_PointLightCount;

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

		for (int i = 0; i < u_PointLightCount; i++) 
		{
			if (!ub_PointLights[i].Enabled)
				continue;

			float dist = distance(ub_PointLights[i].Position.xyz, position);

//			if (dist > ub_PointLights[i].Radius)
//				continue;

			vec3 lightDirection = normalize(ub_PointLights[i].Position.xyz - position);
			
			Output.rgb += max(dot(normal, lightDirection), 0.0) * diffuse * ub_PointLights[i].Color.rgb * ub_PointLights[i].Intensity * (1.0 / (dist * dist));
		
			vec3 halfwayDirection = normalize(lightDirection + viewDirection);
			float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), shininess) * specular;
			Output.rgb += specularStrength * ub_PointLights[i].Color.rgb * 0.25 * ub_PointLights[i].Intensity * (1.0 / (dist * dist));
		}
	}