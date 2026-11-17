#include <string>

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "PointLight.h"
#include "../model/Primitives.h"
#include "../materials/MaterialShaderManager.h"
#include "../materials/LightMaterial.h"

namespace
{
	struct PointLightData
	{
		glm::vec4 Color;
		glm::vec4 Position;
		float Intensity;
		float Radius;
		int Enabled;
	};
}

namespace ultraviolet
{
	void PointLight::Apply(const ShaderStorageBuffer& shaderStorageBuffer, size_t index) const
	{
		PointLightData data{
			.Color{ glm::vec4{ Color.r, Color.g, Color.b, 0.0f } },
			.Position{ glm::vec4{ Position.x, Position.y, Position.z, 0.0f } },
			.Intensity{ Intensity },
			.Radius{ sqrtf(Intensity / 0.01f) },
			.Enabled{ static_cast<int>(Enabled) }
		};

		shaderStorageBuffer.SetData(&data, sizeof(PointLightData), index * sizeof(PointLightData));
	}

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

	size_t PointLight::GetSizeOfData()
	{
		return sizeof(PointLightData);
	}
}