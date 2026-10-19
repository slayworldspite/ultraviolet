#include <imgui/imgui.h>

#include "Scene.h"
#include "../model/ModelLoader.h"
#include "../../physics/EntityLoader.h"

#define MAX_POINT_LIGHT_COUNT 16

namespace ultraviolet
{
	Scene::Scene() :
		Models{},
		AmbientLight{},
		DirectionalLight{},
		PointLights{},
		m_AmbientLightUniformBuffer{ &AmbientLight, sizeof(AmbientLight), 1 },
		m_DirectionalLightUniformBuffer{ &DirectionalLight, DirectionalLight::GetSizeOfData(), 2 },
		m_PointLightsUniformBuffer{ PointLights.data(), PointLight::GetSizeOfData() * MAX_POINT_LIGHT_COUNT + sizeof(int), 3 }
	{

	}

	void Scene::AddModel(const ModelDescription& description)
	{
		auto model{ ModelLoader::Load(description.ID, description.ModelPath) };

		if (model)
		{
			for (Mesh& mesh : model->Meshes)
				mesh.Material = description.Material;

			Models.push_back(std::move(*model));
		}
	}

	void Scene::AddEntity(const EntityDescription& description)
	{
		auto entity{ EntityLoader::Load(description.ID, description.Type, description.ModelPath, description.CollisionModelPath, description.Density, description.Friction) };

		if (entity)
		{
			for (Mesh& mesh : entity->RenderObject.Meshes)
				mesh.Material = description.Material;

			Entities.push_back(std::move(*entity));
		}
	}

	void Scene::AddPointLight(const PointLightDescription& description)
	{
		PointLight light{};
		light.ID = description.ID;
		light.Position = description.Position;
		light.Color = description.Color;
		light.Intensity = description.Intensity;
		light.LinearAttenuation = description.LinearAttenuation;
		light.QuadraticAttenuation = description.QuadraticAttenuation;
		light.Enabled = description.Enabled;

		PointLights.push_back(std::move(light));
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

	Entity* Scene::GetEntity(std::string_view id)
	{
		for (Entity& entity : Entities)
		{
			if (entity.ID == id)
				return &entity;
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
		DirectionalLight.Apply(m_DirectionalLightUniformBuffer);
		
		for (size_t i{}; i < PointLights.size(); i++)
			PointLights[i].Apply(m_PointLightsUniformBuffer, i);

		int maxPointLightIndex = static_cast<int>(PointLights.size());
		m_PointLightsUniformBuffer.SetData(&maxPointLightIndex, sizeof(int), PointLight::GetSizeOfData() * MAX_POINT_LIGHT_COUNT);

		for (Entity& entity : Entities)
			entity.Update();
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

			if (::ImGui::CollapsingHeader("Entities"))
			{
				::ImGui::Indent(16.0f);

				for (Entity& entity : Entities)
					entity.AddToImGui();
			}
		}
	}
}