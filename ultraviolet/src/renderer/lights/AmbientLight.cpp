#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "AmbientLight.h"

namespace
{
	struct AmbientLightData
	{
		glm::vec3 Color;
		float Intensity;
	};
}

namespace ultraviolet
{
	void AmbientLight::Apply(const UniformBuffer& uniformBuffer) const
	{
		AmbientLightData data{ .Color = Color, .Intensity = Intensity };

		uniformBuffer.SetData(&data, sizeof(data));
	}

	void AmbientLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Ambient Light"))
		{
			::ImGui::ColorPicker3("Color ##ambient", glm::value_ptr(Color));
			::ImGui::DragFloat("Intensity ##ambient", &Intensity, 0.01f);
		}
	}
}