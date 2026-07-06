#pragma once

#include "../objects/UniformBuffer.h"

namespace ultraviolet
{
	struct DirectionalLight
	{
		glm::vec3 Color;
		float Intensity{ 1.0f };
		float Yaw;
		float Pitch;
		bool Enabled{ true };

		void Apply(const UniformBuffer& uniformBuffer) const;
		void AddToImGui();

		static size_t GetSizeOfData();
	};
}