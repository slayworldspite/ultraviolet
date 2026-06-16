#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "PointLight.h"

namespace ultraviolet
{
	void PointLight::Apply(const Shader& shader) const
	{
		std::string name{ "u_PointLights[" + std::to_string(Index) + "]" };

		shader.Set(name + ".Position", Position);
		shader.Set(name + ".Color", Color);
		shader.Set(name + ".Intensity", Intensity);
		shader.Set(name + ".Linear", Linear);
		shader.Set(name + ".Quadratic", Quadratic);
		shader.Set(name + ".Enabled", Enabled);
	}

	void PointLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader(("Point Light #" + std::to_string(Index)).c_str()))
		{
			::ImGui::Checkbox(("Enabled ##point" + std::to_string(Index)).c_str(), &Enabled);
			::ImGui::DragFloat3(("Position ##point" + std::to_string(Index)).c_str(), glm::value_ptr(Position), 0.1f);
			::ImGui::ColorPicker3(("Color ##point" + std::to_string(Index)).c_str(), glm::value_ptr(Color));
			::ImGui::DragFloat(("Linear ##point" + std::to_string(Index)).c_str(), &Linear, 0.001f);
			::ImGui::DragFloat(("Quadratic ##point" + std::to_string(Index)).c_str(), &Quadratic, 0.0001f);
			::ImGui::DragFloat(("Intensity ##point" + std::to_string(Index)).c_str(), &Intensity, 0.01f);
		}
	}
}