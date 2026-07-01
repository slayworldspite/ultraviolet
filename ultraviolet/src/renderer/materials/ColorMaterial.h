#pragma once

#include "Material.h"

namespace ultraviolet
{
	class ColorMaterial : public Material
	{
	public:
		glm::vec3 Color;
		int Specular;

	public:
		ColorMaterial();
		ColorMaterial(const glm::vec3& color, int specular);
		virtual ~ColorMaterial() = default;

		virtual void Apply() const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}