#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	struct PointLight
	{
		glm::vec3 Position{};
		glm::vec3 Color{ 1.0f };
		float Intensity{ 1.0f };
		bool Enabled{ true };
		size_t Index{};

		void Apply(const Shader& shader) const;
		void AddToImGui();
	};
}