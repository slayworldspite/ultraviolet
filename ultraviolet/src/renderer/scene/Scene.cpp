#include <imgui/imgui.h>

#include "Scene.h"
#include "../model/ModelLoader.h"
#include "../../physics/EntityLoader.h"

namespace ultraviolet
{
	Scene::Scene() :
		Models{},
		AmbientLight{},
		DirectionalLight{},
		PointLights{},
		m_AmbientLightUniformBuffer{ &AmbientLight, sizeof(AmbientLight), 1 },
		m_DirectionalLightUniformBuffer{ &DirectionalLight, DirectionalLight::GetSizeOfData(), 2 },
		m_PointLightsShaderStorageBuffer{ nullptr, PointLight::GetSizeOfData() * PointLights.size(), 0 },
		m_LastPointLightSize{ PointLights.size() }
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
		
		if (PointLights.size() != m_LastPointLightSize)
		{
			m_PointLightsShaderStorageBuffer = std::move(ShaderStorageBuffer{ nullptr, PointLight::GetSizeOfData() * PointLights.size(), 0 });
			
			m_LastPointLightSize = PointLights.size();
		}

		for (size_t i{}; i < PointLights.size(); i++)
			PointLights[i].Apply(m_PointLightsShaderStorageBuffer, i);

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