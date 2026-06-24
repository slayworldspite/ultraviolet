#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "ColorMaterial.h"

namespace ultraviolet
{
	ColorMaterial::ColorMaterial(const glm::vec3& color, float metallic, float roughness) :
		Metallic{ metallic },
		Roughness{ roughness }
	{

	}

	void ColorMaterial::Apply(const Shader& shader) const
	{
		shader.Use();

		shader.Set("u_ColorMaterial.Color", Color);
		shader.Set("u_ColorMaterial.Metallic", Metallic);
		shader.Set("u_ColorMaterial.Roughness", Roughness);
		shader.Set("u_HasTexture", false);
	}

	void ColorMaterial::Free()
	{

	}

	void ColorMaterial::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("Color Material ##" + id).c_str()))
		{
			::ImGui::ColorPicker3(("Color ##material" + id).c_str(), glm::value_ptr(Color));
			::ImGui::DragFloat(("Metallic ##material" + id).c_str(), &Metallic, 0.01f, 0.0f, 1.0f);
			::ImGui::DragFloat(("Roughness ##material" + id).c_str(), &Roughness, 0.01f, 0.0f, 1.0f);
		}
	}
}