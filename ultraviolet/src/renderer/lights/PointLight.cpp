#include <string>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "PointLight.h"
#include "../model/Primitives.h"
#include "../materials/MaterialShaderManager.h"
#include "../materials/LightMaterial.h"

namespace ultraviolet
{
	void PointLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader(ID.c_str()))
		{
			::ImGui::Checkbox(("Enabled ##point" + ID).c_str(), &Enabled);
			::ImGui::DragFloat3(("Position ##point" + ID).c_str(), glm::value_ptr(Position), 0.1f);
			::ImGui::ColorPicker3(("Color ##point" + ID).c_str(), glm::value_ptr(Color));
			::ImGui::DragFloat(("Intensity ##point" + ID).c_str(), &Intensity, 0.01f);
		}
	}
}