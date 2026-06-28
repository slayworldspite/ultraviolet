#pragma once

#include "../model/Model.h"
#include "../lights/AmbientLight.h"
#include "../lights/DirectionalLight.h"
#include "../lights/PointLight.h"

namespace ultraviolet
{
	class Scene
	{
	public:
		std::vector<Model> Models;
		AmbientLight AmbientLight;
		DirectionalLight DirectionalLight;
		std::vector<PointLight> PointLights;

	public:
		Model* GetModel(std::string_view id);
		PointLight* GetPointLight(std::string_view id);

		void AddToImGui();
	};
}