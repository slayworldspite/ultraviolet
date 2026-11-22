#pragma once

#include <filesystem>

#include "../model/Model.h"
#include "../../physics/Entity.h"
#include "../lights/AmbientLight.h"
#include "../lights/DirectionalLight.h"
#include "../lights/PointLight.h"
#include "../lights/LightingData.h"
#include "../camera/ProjectionCamera.h"
#include "../objects/ShaderStorageBuffer.h"
#include "descriptions/ModelDescription.h"
#include "descriptions/EntityDescription.h"

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

		void AddModel(const ModelDescription& description);
		void AddEntity(const EntityDescription& description);
		Model* GetModel(std::string_view id);
		Entity* GetEntity(std::string_view id);
		PointLight* GetPointLight(std::string_view id);
		LightingData GetLightingData() const;
		void Update();
		void Clear();

		void AddToImGui();
	};
}