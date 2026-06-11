#include "TextureMaterial.h"

namespace ultraviolet
{
	TextureMaterial::TextureMaterial(const Texture& texture) :
		Textures{ texture }
	{

	}

	TextureMaterial::TextureMaterial(const std::vector<Texture>& textures) :
		Textures{ textures }
	{

	}

	void TextureMaterial::Apply(const Shader& shader) const
	{
		for (size_t i{}; i < Textures.size(); i++)
		{
			Textures[i].Bind(i);
		}
	}
}