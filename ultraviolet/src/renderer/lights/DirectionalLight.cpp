#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"

namespace ultraviolet
{
	void DirectionalLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Directional Light"))
		{
			::ImGui::Checkbox("Enabled ##directional", &Enabled);
			::ImGui::ColorPicker3("Color ##directional", glm::value_ptr(Color));
			::ImGui::DragFloat("Yaw ##directional", &Yaw, 0.1f);
			::ImGui::DragFloat("Pitch ##directional", &Pitch, 0.1f);
			::ImGui::DragFloat("Intensity ##directional", &Intensity, 0.1f, 0.0f, 10.0f);
		}
	}
}