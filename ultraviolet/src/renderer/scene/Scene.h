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
		void Bind() const;
		Model* GetModel();
	};
}