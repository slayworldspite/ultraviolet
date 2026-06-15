#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	struct AmbientLight
	{
		glm::vec3 Color{};
		float Intensity{ 1.0f };

		void Apply(const Shader& shader) const;
		void AddToImGui();
	};
}