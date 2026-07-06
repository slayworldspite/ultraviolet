#include <imgui/imgui.h>

#include "Scene.h"

#define MAX_POINT_LIGHT_COUNT 16

namespace ultraviolet
{
	Scene::Scene() :
		Models{},
		AmbientLight{},
		DirectionalLight{},
		PointLights{},
		m_AmbientLightUniformBuffer{ &AmbientLight, sizeof(AmbientLight), 1 },
		m_PointLightsUniformBuffer{ PointLights.data(), PointLight::GetSizeOfData() * MAX_POINT_LIGHT_COUNT + sizeof(int), 2}
	{

	}

	Model* Scene::GetModel(std::string_view id)
	{
		for (Model& model : Models)
		{
			if (model.ID == id)
				return &model;
		}
		
		return nullptr;
	}

	PointLight* Scene::GetPointLight(std::string_view id)
	{
		for (PointLight& pointLight : PointLights)
		{
			if (pointLight.ID == id)
				return &pointLight;
		}

		return nullptr;
	}

	void Scene::Update()
	{
		AmbientLight.Apply(m_AmbientLightUniformBuffer);
		
		for (size_t i{}; i < PointLights.size(); i++)
			PointLights[i].Apply(m_PointLightsUniformBuffer, i);

		int maxPointLightIndex = static_cast<int>(PointLights.size());
		m_PointLightsUniformBuffer.SetData(&maxPointLightIndex, sizeof(int), PointLight::GetSizeOfData() * MAX_POINT_LIGHT_COUNT);
	}

	void Scene::Clear()
	{
		PointLights.clear();
		Models.clear();

		AmbientLight.Color = glm::vec3{ 1.0f };
		AmbientLight.Intensity = 0.0f;

		DirectionalLight.Color = glm::vec3{ 1.0f };
		DirectionalLight.Intensity = 1.0f;
		DirectionalLight.Enabled = false;
		DirectionalLight.Yaw = 0.0f;
		DirectionalLight.Pitch = 0.0f;
	}

	void Scene::AddToImGui()
	{
		if (::ImGui::CollapsingHeader("Scene"))
		{
			::ImGui::Indent(16.0f);

			AmbientLight.AddToImGui();
			DirectionalLight.AddToImGui();
			
			if (::ImGui::CollapsingHeader("Point Lights"))
			{
				::ImGui::Indent(16.0f);

				static size_t lastPointLightIndex{ PointLights.size() };

				if (::ImGui::Button("Push Point Light"))
				{
					PointLights.emplace_back(std::to_string(lastPointLightIndex));

					lastPointLightIndex++;
				}

				if (::ImGui::Button("Pop Point Light") && !PointLights.empty())
				{
					PointLights.pop_back();
					lastPointLightIndex--;
				}

				for (PointLight& pointLight : PointLights)
					pointLight.AddToImGui();

				::ImGui::Indent(-16.0f);
			}

			if (::ImGui::CollapsingHeader("Models"))
			{
				::ImGui::Indent(16.0f);

				for (Model& model : Models)
					model.AddToImGui();
			}
		}
	}
}