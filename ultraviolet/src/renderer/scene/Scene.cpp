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
}