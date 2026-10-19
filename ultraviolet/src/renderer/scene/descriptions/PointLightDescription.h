#pragma once

#include <string>

#include <glm/glm.hpp>

namespace ultraviolet
{
	struct PointLightDescription
	{
		std::string ID;
		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity = 1.0f;
		float LinearAttenuation = 0.02f;
		float QuadraticAttenuation = 0.032f;
		bool Enabled = true;
	};
}