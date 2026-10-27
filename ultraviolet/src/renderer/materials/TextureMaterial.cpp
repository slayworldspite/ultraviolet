#include <imgui/imgui.h>
#include <imgui/implot.h>

#include "TextureMaterial.h"
#include "MaterialShaderManager.h"

namespace ultraviolet
{
	TextureMaterial::TextureMaterial() :
		Material{ MaterialType::Texture },
		Diffuse{ nullptr },
		Specular{ nullptr },
		Normal{ nullptr },
		Shininess{ 32 },
		TextureScale{ 1.0f },
		NormalStrength{ 1.0f },
		FlipNormals{ false }
	{

	}

	TextureMaterial::TextureMaterial(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> specular, std::shared_ptr<Texture> normal, int shininess, float textureScale, float normalStrength, bool flipNormals) :
		Material{ MaterialType::Texture },
		Diffuse{ diffuse },
		Specular{ specular },
		Normal{ normal },
		Shininess{ shininess },
		TextureScale{ textureScale },
		NormalStrength{ normalStrength },
		FlipNormals{ flipNormals }
	{

	}

	TextureMaterial::TextureMaterial(const std::filesystem::path& diffusePath, const std::filesystem::path& specularPath, const std::filesystem::path& normalPath, int shininess, float textureScale, float normalStrength, bool flipNormals) :
		Material{ MaterialType::Texture },
		Diffuse{ std::make_shared<Texture>(diffusePath, TextureType::Diffuse)},
		Specular{ std::make_shared<Texture>(specularPath, TextureType::Specular) },
		Normal{ std::make_shared<Texture>(normalPath, TextureType::Normal) },
		Shininess{ shininess },
		TextureScale{ textureScale },
		NormalStrength{ normalStrength },
		FlipNormals{ flipNormals }
	{

	}

	TextureMaterial::~TextureMaterial()
	{
		Free();
	}

	void TextureMaterial::Apply() const
	{
		Shader* shader{ MaterialShaderManager::Get(Type) };

		shader->Set("u_TextureMaterial.Diffuse", 0);
		shader->Set("u_TextureMaterial.Specular", 1);
		shader->Set("u_TextureMaterial.Normal", 2);
		shader->Set("u_TextureMaterial.Shininess", Shininess);
		shader->Set("u_TextureMaterial.TextureScale", TextureScale);
		shader->Set("u_TextureMaterial.NormalStrength", NormalStrength);
		shader->Set("u_TextureMaterial.FlipNormals", FlipNormals);

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
			::ImGui::DragFloat(("Texture Scale ##" + id).c_str(), &TextureScale, 0.1f);
			::ImGui::DragFloat(("Normal Strength ##" + id).c_str(), &NormalStrength, 0.05f);
			::ImGui::Checkbox(("Flip Normals ##" + id).c_str(), &FlipNormals);

			if (::ImGui::CollapsingHeader(("Textures ##" + id).c_str()))
			{
				if (::ImPlot::BeginPlot(("Diffuse Texture ##" + id).c_str()))
				{
					::ImPlot::PlotImage(("Diffuse ##" + id).c_str(),
						(ImTextureID)(intptr_t)Diffuse->GetID(),
						ImVec2{ 0, 0 }, ImVec2{ 1, 1 }
					);

					::ImPlot::EndPlot();
				}

				if (::ImPlot::BeginPlot(("Specular Texture ##" + id).c_str()))
				{
					::ImPlot::PlotImage(("Specular ##" + id).c_str(),
						(ImTextureID)(intptr_t)Specular->GetID(),
						ImVec2{ 0, 0 }, ImVec2{ 1, 1 }
					);

					::ImPlot::EndPlot();
				}

				if (::ImPlot::BeginPlot(("Normal Texture ##" + id).c_str()))
				{
					::ImPlot::PlotImage(("Normal ##" + id).c_str(),
						(ImTextureID)(intptr_t)Normal->GetID(),
						ImVec2{ 0, 0 }, ImVec2{ 1, 1 }
					);

					::ImPlot::EndPlot();
				}
			}
		}
	}
}