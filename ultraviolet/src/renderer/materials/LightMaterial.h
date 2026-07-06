#pragma once

#include <glm/glm.hpp>

#include "Material.h"

namespace ultraviolet
{
	class LightMaterial : public Material
	{
	public:
		glm::vec3 Color;
		bool Enabled;

	public:
		LightMaterial();
		LightMaterial(const glm::vec3& color);
		virtual ~LightMaterial() = default;

		virtual void Apply() const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}