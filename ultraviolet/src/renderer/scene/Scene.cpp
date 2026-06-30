#include <imgui/imgui.h>

#include "Scene.h"

namespace ultraviolet
{
	Scene::Scene() :
		Models{},
		AmbientLight{},
		DirectionalLight{},
		PointLights{},
		m_AmbientLightUniformBuffer{ &AmbientLight, sizeof(AmbientLight), 1 }
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
					
					PointLights.emplace_back();
					PointLights[lastPointLightIndex - 1].Index = lastPointLightIndex++;
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