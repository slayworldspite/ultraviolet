#pragma once

#include "IMaterial.h"
#include "../objects/Texture.h"

namespace ultraviolet
{
	class TextureMaterial : public IMaterial
	{
	public:
		std::vector<std::shared_ptr<Texture>> Textures;

	public:
		TextureMaterial(std::shared_ptr<Texture> texture);
		TextureMaterial(const std::vector<std::shared_ptr<Texture>>& textures);

		virtual void Apply(const Shader& shader) const override;
		virtual void Free() override;
	};
}