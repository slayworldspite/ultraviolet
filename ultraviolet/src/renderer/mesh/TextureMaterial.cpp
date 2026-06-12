#include "TextureMaterial.h"

namespace ultraviolet
{
	TextureMaterial::TextureMaterial(std::shared_ptr<Texture> texture) :
		Textures{ texture }
	{

	}

	TextureMaterial::TextureMaterial(const std::vector<std::shared_ptr<Texture>>& textures) :
		Textures{ textures }
	{

	}

	void TextureMaterial::Apply(const Shader& shader) const
	{
		shader.Set("u_HasTexture", true);

		for (size_t i{}; i < Textures.size(); i++)
		{
			Textures[i]->Bind(i);
		}
	}

	void TextureMaterial::Free()
	{
		for (size_t i{ Textures.size() }; i-- > 0;)
		{
			Textures[i]->Delete();
			Textures.erase(Textures.begin() + i);
		}
	}
}