#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "ColorMaterial.h"

namespace ultraviolet
{
	ColorMaterial::ColorMaterial(const glm::vec4& color, int specular) :
		Color{ color },
		Specular{ specular }
	{

	}

	void ColorMaterial::Apply(const Shader& shader) const
	{
		shader.Use();
		shader.Set("u_ColorMaterial.Color", Color);
		shader.Set("u_ColorMaterial.Specular", Specular);
		shader.Set("u_HasTexture", false);
	}

	void ColorMaterial::Free()
	{

	}

	void ColorMaterial::AddToImGui(const std::string& id, size_t index)
	{
		if (::ImGui::CollapsingHeader(("Color Material ##" + id + std::to_string(index)).c_str()))
		{
			::ImGui::ColorPicker4(("Color ##material" + id + std::to_string(index)).c_str(), glm::value_ptr(Color));
			::ImGui::DragInt(("Specular ##material" + id + std::to_string(index)).c_str(), &Specular, 1.0f, 4, 256);
		}
	}
}