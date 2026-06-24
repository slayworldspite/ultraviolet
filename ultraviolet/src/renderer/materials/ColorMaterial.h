#pragma once

#include "IMaterial.h"

namespace ultraviolet
{
	class ColorMaterial : public IMaterial
	{
	public:
		glm::vec3 Color;
		float Metallic;
		float Roughness;

	public:
		ColorMaterial(const glm::vec3& color, float metallic, float roughness);
		virtual ~ColorMaterial() = default;

		virtual void Apply(const Shader& shader) const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}