#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "AmbientLight.h"

namespace ultraviolet
{
	void AmbientLight::Apply(const Shader& shader) const
	{
		shader.Set("u_AmbientLight.Color", Color);
		shader.Set("u_AmbientLight.Intensity", Intensity);
	}

	void AmbientLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Ambient Light"))
		{
			::ImGui::ColorPicker3("Ambient Light Color", glm::value_ptr(Color));
			::ImGui::DragFloat("Ambient Light Intensity", &Intensity, 0.01f);
		}
	}
}