#pragma once

#include "../objects/UniformBuffer.h"

namespace ultraviolet
{
	struct AmbientLight
	{
		glm::vec3 Color;
		float Intensity{ 1.0f };

		void AddToImGui();
	};
}