#pragma once

#include <glm/glm.hpp>

#include "IMaterial.h"

namespace ultraviolet
{
	class ColorMaterial : public IMaterial
	{
	public:
		glm::vec4 Color;

	public:
		ColorMaterial(glm::vec4 color);
		ColorMaterial(float r, float g, float b, float a);
		virtual ~ColorMaterial() = default;

		virtual void Apply(const Shader& shader) const override;
	};
}