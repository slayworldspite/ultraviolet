#pragma once

#include "IMaterial.h"

namespace ultraviolet
{
	class ColorMaterial : public IMaterial
	{
	public:
		glm::vec4 Color;
		int Specular;

	public:
		ColorMaterial(const glm::vec4& color, int specular);
		virtual ~ColorMaterial() = default;

		virtual void Apply(const Shader& shader) const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id, size_t index) override;
	};
}