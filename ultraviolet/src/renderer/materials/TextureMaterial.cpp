#include <imgui/imgui.h>

#include "TextureMaterial.h"

namespace ultraviolet
{

	void TextureMaterial::Apply(const Shader& shader) const
	{
		shader.Use();

		shader.Set("u_HasTexture", true);

		shader.Set("u_TextureMaterial.Albedo", 0);
		shader.Set("u_TextureMaterial.Metallic", 1);
		shader.Set("u_TextureMaterial.Roughness", 2);
		shader.Set("u_TextureMaterial.Normal", 3);
		shader.Set("u_TextureMaterial.AmbientOcclusion", 4);

		if (Albedo)
		{
			shader.Set("u_TextureMaterial.HasAlbedo", true);
			Albedo->Bind(0);
		}
		else
		{
			shader.Set("u_TextureMaterial.HasAlbedo", false);
		}

		if (Metallic)
		{
			shader.Set("u_TextureMaterial.HasMetallic", true);
			Metallic->Bind(1);
		}
		else
		{
			shader.Set("u_TextureMaterial.HasMetallic", false);
		}

		if (Roughness)
		{
			shader.Set("u_TextureMaterial.HasRoughness", true);
			Roughness->Bind(2);
		}
		else
		{
			shader.Set("u_TextureMaterial.HasRoughness", false);
		}

		if (Normal)
		{
			shader.Set("u_TextureMaterial.HasNormal", true);
			Normal->Bind(3);
		}
		else
		{
			shader.Set("u_TextureMaterial.HasNormal", false);
		}

		if (AmbientOcclusion)
		{
			shader.Set("u_TextureMaterial.HasAmbientOcclusion", true);
			AmbientOcclusion->Bind(4);
		}
		else
		{
			shader.Set("u_TextureMaterial.HasAmbientOcclusion", false);
		}
	}

	void TextureMaterial::Free()
	{
		if (Albedo)
			Albedo->Delete();

		if (Metallic)
			Metallic->Delete();

		if (Roughness)
			Roughness->Delete();

		if (Normal)
			Normal->Delete();

		if (AmbientOcclusion)
			AmbientOcclusion->Delete();
	}

	void TextureMaterial::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader("Texture Material"))
		{
			::ImGui::Text("Albedo: %s", Albedo ? "true" : "false");
			::ImGui::Text("Metallic: %s", Metallic ? "true" : "false");
			::ImGui::Text("Roughness: %s", Roughness ? "true" : "false");
			::ImGui::Text("Normal: %s", Normal ? "true" : "false");
			::ImGui::Text("Ambient Occlusion: %s", AmbientOcclusion ? "true" : "false");
		}
	}
}