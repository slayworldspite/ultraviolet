#pragma once

#include "../model/Model.h"
#include "../objects/ShaderStorageBuffer.h"

namespace ultraviolet
{
	struct PointLight
	{
		std::string ID;
		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity{ 1.0f };
		bool Enabled{ true };

		void AddToImGui();
	};
}