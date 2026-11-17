#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"

namespace
{
	struct DirectionalLightData
	{
		glm::vec4 Color;
		glm::vec4 Direction;
		float Intensity;
		int Enabled;

	private:
		uint8_t m_Pad[8];
	};
}

namespace ultraviolet
{
	void DirectionalLight::Apply(const UniformBuffer& uniformBuffer) const
	{
		DirectionalLightData data{};

		data.Color = glm::vec4{ Color.r, Color.g, Color.b, 0.0f };

		data.Direction.x = glm::cos(glm::radians(Pitch)) * glm::sin(glm::radians(Yaw));
		data.Direction.y = glm::sin(glm::radians(Pitch));
		data.Direction.z = glm::cos(glm::radians(Pitch)) * glm::cos(glm::radians(Yaw));

		data.Direction = glm::normalize(data.Direction);

		data.Intensity = Intensity;
		data.Enabled = Enabled;

		uniformBuffer.SetData(&data, sizeof(data));
	}
	
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

	size_t DirectionalLight::GetSizeOfData()
	{
		return sizeof(DirectionalLightData);
	}
}