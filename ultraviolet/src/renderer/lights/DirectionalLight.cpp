#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"

namespace ultraviolet
{
	void DirectionalLight::Apply(const Shader& shader) const
	{
		shader.Use();

		glm::vec3 direction{};

		direction.x = glm::cos(glm::radians(Pitch)) * glm::sin(glm::radians(Yaw));
		direction.y = glm::sin(glm::radians(Pitch));
		direction.z = glm::sin(glm::radians(Pitch)) * glm::cos(glm::radians(Yaw));

		shader.Set("u_DirectionalLight.Color", Color);
		shader.Set("u_DirectionalLight.Direction", direction);
		shader.Set("u_DirectionalLight.Intensity", Intensity);
		shader.Set("u_DirectionalLight.Enabled", Enabled);
	}
	
	void DirectionalLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Directional Light"))
		{
			::ImGui::Checkbox("Enabled ##directional", &Enabled);
			::ImGui::ColorPicker3("Color ##directional", glm::value_ptr(Color));
			::ImGui::DragFloat("Yaw ##directional", &Yaw, 0.1f);
			::ImGui::DragFloat("Pitch ##directional", &Pitch, 0.1f);
			::ImGui::DragFloat("Intensity ##directional", &Intensity, 0.1f, 0.0f, 1.0f);
		}
	}
}