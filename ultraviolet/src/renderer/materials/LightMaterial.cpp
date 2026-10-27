#include "LightMaterial.h"
#include "MaterialShaderManager.h"

namespace ultraviolet
{
	LightMaterial::LightMaterial() :
		Material{ MaterialType::Light },
		Color{ 1.0f },
		Intensity{ 1.0f },
		Enabled{ true }
	{

	}

	LightMaterial::LightMaterial(const glm::vec3& color) :
		Material{ MaterialType::Light },
		Color{ color },
		Intensity{ 1.0f },
		Enabled{ true }
	{

	}

	void LightMaterial::Apply() const
	{
		Shader* shader{ MaterialShaderManager::Get(Type) };

		shader->Set("u_LightMaterial.Color", Color);
		shader->Set("u_LightMaterial.Intensity", Intensity);
		shader->Set("u_LightMaterial.Enabled", Enabled);
	}

	void LightMaterial::Free()
	{

	}

	void LightMaterial::AddToImGui(const std::string& id)
	{

	}
}