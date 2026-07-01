#pragma once

#include "Material.h"
#include "../objects/Texture.h"

namespace ultraviolet
{
	class TextureMaterial : public Material
	{
	public:
		std::shared_ptr<Texture> Diffuse;
		std::shared_ptr<Texture> Specular;
		int Shininess;

	public:
		TextureMaterial();
		TextureMaterial(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, int shininess);
		TextureMaterial(const std::filesystem::path& diffusePath, const std::filesystem::path& specularPath, int shininess);
		virtual ~TextureMaterial() override;

		virtual void Apply() const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}