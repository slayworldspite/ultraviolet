#pragma once

#include "IMaterial.h"

namespace ultraviolet
{
	class ColorMaterial : public IMaterial
	{
	public:
		glm::vec4 Color;

	public:
		ColorMaterial(const glm::vec4& color);

		virtual void Apply(const Shader& shader) const override;
	};
}