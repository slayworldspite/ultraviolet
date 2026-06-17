#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "AmbientLight.h"

namespace ultraviolet
{
	void AmbientLight::Apply(const Shader& shader) const
	{
		shader.Use();

		shader.Set("u_AmbientLight.Color", Color);
		shader.Set("u_AmbientLight.Intensity", Intensity);
		shader.Set("u_AmbientLight.Enabled", Enabled);
	}

	void AmbientLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Ambient Light"))
		{
			::ImGui::Checkbox("Enabled ##ambient", &Enabled);
			::ImGui::ColorPicker3("Color ##ambient", glm::value_ptr(Color));
			::ImGui::DragFloat("Intensity ##ambient", &Intensity, 0.01f);
		}
	}
}