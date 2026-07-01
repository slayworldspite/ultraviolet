#include <imgui/imgui.h>

#include "TextureMaterial.h"

namespace ultraviolet
{
	TextureMaterial::TextureMaterial() :
		Material{ MaterialType::Texture },
		Diffuse{ nullptr },
		Specular{ nullptr },
		Shininess{ 32 }
	{

	}

	TextureMaterial::TextureMaterial(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, int shininess) :
		Material{ MaterialType::Texture },
		Diffuse{ diffuse },
		Specular{ specular },
		Shininess{ shininess }
	{

	}

	TextureMaterial::TextureMaterial(const std::filesystem::path& diffusePath, const std::filesystem::path& specularPath, int shininess) :
		Material{ MaterialType::Texture },
		Diffuse{ std::make_shared<Texture>(diffusePath, TextureType::Diffuse)},
		Specular{ std::make_shared<Texture>(specularPath, TextureType::Specular) },
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
		Shader->Set("u_TextureMaterial.Shininess", Shininess);

		if (Diffuse)
			Diffuse->Bind(0);

		if (Specular)
			Specular->Bind(1);
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
			::ImGui::Text("Diffuse: %s", Diffuse ? "loaded" : "not loaded");
			::ImGui::Text("Specular: %s", Specular ? "loaded" : "not loaded");
			::ImGui::DragInt(("Shininess ##" + id).c_str(), &Shininess, 1.0f, 0, 256);
		}
	}
}