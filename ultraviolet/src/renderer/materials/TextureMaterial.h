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
		std::shared_ptr<Texture> Normal;
		int Shininess;
		float NormalStrength;
		bool FlipNormals;

	public:
		TextureMaterial();
		TextureMaterial(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, std::shared_ptr<Texture> normal, int shininess, float normalStrength = 1.0f, bool flipNormals = false);
		TextureMaterial(const std::filesystem::path& diffusePath, const std::filesystem::path& specularPath, const std::filesystem::path& normalPath, int shininess, float normalStrength = 1.0f, bool flipNormals = false);
		virtual ~TextureMaterial() override;

		virtual void Apply() const override;
		virtual void Free() override;
		virtual void AddToImGui(const std::string& id) override;
	};
}