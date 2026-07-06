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
		float LinearAttenuation;
		float QuadraticAttenuation;
		int Enabled;
	};
}

namespace ultraviolet
{
	PointLight::PointLight() :
		ID{ "Point Light" },
		Position{ 0.0f },
		Color{ 1.0f },
		Intensity{ 1.0f },
		LinearAttenuation{ 0.02f },
		QuadraticAttenuation{ 0.032f },
		Enabled{ true }
	{

	}

	PointLight::PointLight(std::string_view id) :
		ID{ id },
		Position{ 0.0f },
		Color{ 1.0f },
		Intensity{ 1.0f },
		LinearAttenuation{ 0.02f },
		QuadraticAttenuation{ 0.032f },
		Enabled{ true }
	{

	}

	void PointLight::Apply(const UniformBuffer& uniformBuffer, size_t index) const
	{
		PointLightData data{
			.Color{ glm::vec4{ Color.r, Color.g, Color.b, 0.0f } },
			.Position{ glm::vec4{ Position.x, Position.y, Position.z, 0.0f } },
			.Intensity{ Intensity },
			.LinearAttenuation{ LinearAttenuation },
			.QuadraticAttenuation{ QuadraticAttenuation },
			.Enabled{ static_cast<int>(Enabled) }
		};

		uniformBuffer.SetData(&data, sizeof(data), index * sizeof(data));
	}

	void PointLight::AddToImGui()
	{
		if (::ImGui::CollapsingHeader(ID.c_str()))
		{
			::ImGui::Checkbox(("Enabled ##point" + ID).c_str(), &Enabled);
			::ImGui::DragFloat3(("Position ##point" + ID).c_str(), glm::value_ptr(Position), 0.1f);
			::ImGui::ColorPicker3(("Color ##point" + ID).c_str(), glm::value_ptr(Color));
			::ImGui::DragFloat(("Intensity ##point" + ID).c_str(), &Intensity, 0.01f);
			::ImGui::DragFloat(("Linear Attenuation ##point" + ID).c_str(), &LinearAttenuation, 0.0001f);
			::ImGui::DragFloat(("Quadratic Attenuation ##point" + ID).c_str(), &QuadraticAttenuation, 0.0001f);
		}
	}

	size_t PointLight::GetSizeOfData()
	{
		return sizeof(PointLightData);
	}
}