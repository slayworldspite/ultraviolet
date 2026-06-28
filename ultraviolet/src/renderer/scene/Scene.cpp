#include <imgui/imgui.h>

#include "Scene.h"

namespace ultraviolet
{
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

	void Scene::AddToImGui()
	{
		if (ImGui::CollapsingHeader("Scene"))
		{
			AmbientLight.AddToImGui();
			DirectionalLight.AddToImGui();
			
			if (ImGui::CollapsingHeader("Point Lights"))
			{
				if (ImGui::Button("Push Point Light"))
					PointLights.emplace_back();

				if (ImGui::Button("Pop Point Light"))
					PointLights.pop_back();

				for (PointLight& pointLight : PointLights)
					pointLight.AddToImGui();
			}

			if (ImGui::CollapsingHeader("Models"))
			{
				for (Model& model : Models)
					model.AddToImGui();
			}
		}
	}
}