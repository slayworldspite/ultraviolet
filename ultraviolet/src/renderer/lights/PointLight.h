#pragma once

#include "../model/Model.h"
#include "../objects/UniformBuffer.h"

namespace ultraviolet
{
	struct PointLight
	{
		std::string ID;
		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity;
		float LinearAttenuation;
		float QuadraticAttenuation;
		bool Enabled;

		PointLight();
		PointLight(std::string_view id);

		void Apply(const UniformBuffer& uniformBuffer, size_t index) const;
		void AddToImGui();

		static size_t GetSizeOfData();
	};
}