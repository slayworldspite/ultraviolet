#include <unordered_map>

#include "MaterialShaderManager.h"

namespace ultraviolet
{
	std::unordered_map<MaterialType, std::shared_ptr<Shader>> m_Shaders{};

	void MaterialShaderManager::Initialize()
	{

	}

	std::shared_ptr<Shader> MaterialShaderManager::Get(MaterialType type)
	{
		return nullptr;
	}
}