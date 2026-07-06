#include "LightMaterial.h"

namespace ultraviolet
{
	LightMaterial::LightMaterial() :
		Material{ MaterialType::Light },
		Color{ 1.0f },
		Enabled{ true }
	{

	}

	LightMaterial::LightMaterial(const glm::vec3& color) :
		Material{ MaterialType::Light },
		Color{ color },
		Enabled{ true }
	{

	}

	void LightMaterial::Apply() const
	{
		Shader->Set("u_LightMaterial.Color", Color);
		Shader->Set("u_LightMaterial.Enabled", Enabled);
	}

	void LightMaterial::Free()
	{

	}

	void LightMaterial::AddToImGui(const std::string& id)
	{

	}
}