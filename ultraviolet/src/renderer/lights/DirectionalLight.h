#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	struct DirectionalLight
	{
		glm::vec3 Color;
		float Intensity{ 1.0f };
		float Yaw;
		float Pitch;
		bool Enabled{ true };

		void Apply(const Shader& shader) const;
		void AddToImGui();
	};
}