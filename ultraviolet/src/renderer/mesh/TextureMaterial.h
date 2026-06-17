#pragma once

#include "IMaterial.h"
#include "../objects/Texture.h"

namespace ultraviolet
{
	class TextureMaterial : public IMaterial
	{
	public:
		std::shared_ptr<Texture> Albedo;
		std::shared_ptr<Texture> Metallic;
		std::shared_ptr<Texture> Roughness;
		std::shared_ptr<Texture> Normal;
		std::shared_ptr<Texture> AmbientOcclusion;

	public:
		virtual void Apply(const Shader& shader) const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id, size_t index) override;
	};
}