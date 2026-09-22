#pragma once

#include "../model/Model.h"
#include "../../physics/Entity.h"
#include "../lights/AmbientLight.h"
#include "../lights/DirectionalLight.h"
#include "../lights/PointLight.h"
#include "../camera/ProjectionCamera.h"

namespace ultraviolet
{
	class Scene
	{
	public:
		std::vector<Model> Models;
		std::vector<Entity> Entities;
		AmbientLight AmbientLight;
		DirectionalLight DirectionalLight;
		std::vector<PointLight> PointLights;
		ProjectionCamera Camera;

	public:
		Scene();

		Model* GetModel(std::string_view id);
		Entity* GetEntity(std::string_view id);
		PointLight* GetPointLight(std::string_view id);
		void Update();
		void Clear();

		void AddToImGui();

	private:
		UniformBuffer m_AmbientLightUniformBuffer;
		UniformBuffer m_DirectionalLightUniformBuffer;
		UniformBuffer m_PointLightsUniformBuffer;
	};
}