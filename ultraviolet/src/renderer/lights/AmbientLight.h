#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	struct AmbientLight
	{
		glm::vec3 Color;
		float Intensity;

		void Apply(const Shader& shader) const;
		void AddToImGui();
	};
}