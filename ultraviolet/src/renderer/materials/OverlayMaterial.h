#pragma once

#include "Material.h"
#include "../objects/Texture.h"

namespace ultraviolet
{
	class OverlayMaterial : public Material
	{
	public:
		std::shared_ptr<Texture> Overlay;

	public:
		OverlayMaterial(std::shared_ptr<Texture> overlay);
		virtual ~OverlayMaterial() override;

		virtual void Apply() const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}