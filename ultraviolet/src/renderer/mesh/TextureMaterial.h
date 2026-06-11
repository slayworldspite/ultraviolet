#pragma once

#include "IMaterial.h"
#include "../objects/Texture.h"

namespace ultraviolet
{
	class TextureMaterial : public IMaterial
	{
	public:
		std::vector<Texture> Textures;

	public:
		TextureMaterial(const Texture& texture);
		TextureMaterial(const std::vector<Texture>& textures);

		virtual void Apply(const Shader& shader) const override;
	};
}