#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "AmbientLight.h"

namespace ultraviolet
{
	void AmbientLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Ambient Light"))
		{
			::ImGui::ColorPicker3("Color ##ambient", glm::value_ptr(Color));
			::ImGui::DragFloat("Intensity ##ambient", &Intensity, 0.01f);
		}
	}
}