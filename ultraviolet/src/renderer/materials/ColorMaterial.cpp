#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "ColorMaterial.h"
#include "MaterialShaderManager.h"

namespace ultraviolet
{
	ColorMaterial::ColorMaterial() :
		Material{ MaterialType::Color },
		Color{ 1.0f },
		Specular{ 32 }
	{

	}

	ColorMaterial::ColorMaterial(const glm::vec3& color, int specular) :
		Material{ MaterialType::Color },
		Color{ color },
		Specular{ specular }
	{

	}

	void ColorMaterial::Apply() const
	{
		Shader* shader{ MaterialShaderManager::Get(Type) };

		shader->Use();
		
		shader->Set("u_ColorMaterial.Color", Color);
		shader->Set("u_ColorMaterial.Specular", Specular);
	}

	void ColorMaterial::Free()
	{

	}

	void ColorMaterial::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("Color Material ##" + id).c_str()))
		{
			::ImGui::ColorPicker3(("Color ##material" + id).c_str(), glm::value_ptr(Color));
			::ImGui::DragInt(("Specular ##material" + id).c_str(), &Specular, 1.0f, 0, 256);
		}
	}
}