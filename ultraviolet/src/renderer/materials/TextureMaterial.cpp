#include <imgui/imgui.h>

#include "TextureMaterial.h"

namespace ultraviolet
{
	TextureMaterial::TextureMaterial() :
		Material{ MaterialType::Texture },
		Diffuse{ nullptr },
		Specular{ nullptr },
		Normal{ nullptr },
		Shininess{ 32 }
	{

	}

	TextureMaterial::TextureMaterial(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, std::shared_ptr<Texture> normal, int shininess) :
		Material{ MaterialType::Texture },
		Diffuse{ diffuse },
		Specular{ specular },
		Shininess{ shininess },
		Normal{ normal }
	{

	}

	TextureMaterial::TextureMaterial(const std::filesystem::path& diffusePath, const std::filesystem::path& specularPath, const std::filesystem::path& normalPath, int shininess) :
		Material{ MaterialType::Texture },
		Diffuse{ std::make_shared<Texture>(diffusePath, TextureType::Diffuse)},
		Specular{ std::make_shared<Texture>(specularPath, TextureType::Specular) },
		Normal{ std::make_shared<Texture>(normalPath, TextureType::Normal) },
		Shininess{ shininess }
	{

	}

	TextureMaterial::~TextureMaterial()
	{
		Free();
	}

	void TextureMaterial::Apply() const
	{
		Shader->Set("u_TextureMaterial.Diffuse", 0);
		Shader->Set("u_TextureMaterial.Specular", 1);
		Shader->Set("u_TextureMaterial.Normal", 2);
		Shader->Set("u_TextureMaterial.Shininess", Shininess);

		if (Diffuse)
			Diffuse->Bind(0);

		if (Specular)
			Specular->Bind(1);

		if (Normal)
			Normal->Bind(2);
	}

	void TextureMaterial::Free()
	{
		if (Diffuse)
			Diffuse->Delete();

		if (Specular)
			Specular->Delete();
	}

	void TextureMaterial::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("TextureMaterial ##" + id).c_str()))
		{
			::ImGui::DragInt(("Shininess ##" + id).c_str(), &Shininess, 1.0f, 0, 256);

			auto diffuseSize{ Diffuse->GetSize() };
			::ImGui::Image((ImTextureID)(intptr_t)Diffuse->GetID(), ImVec2{ static_cast<float>(diffuseSize.first), static_cast<float>(diffuseSize.second) });

			auto specularSize{ Specular->GetSize() };
			::ImGui::Image((ImTextureID)(intptr_t)Specular->GetID(), ImVec2{ static_cast<float>(specularSize.first), static_cast<float>(specularSize.second) });

			auto normalSize{ Normal->GetSize() };
			::ImGui::Image((ImTextureID)(intptr_t)Normal->GetID(), ImVec2{ static_cast<float>(normalSize.first), static_cast<float>(normalSize.second) });
		}
	}
}