#include <unordered_map>

#include "MaterialShaderManager.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	bool s_IsInitialized{ false };
	std::unordered_map<MaterialType, std::shared_ptr<Shader>> s_Shaders{};

	void MaterialShaderManager::Initialize()
	{
		Logger::Log("initialzing material shaders...", Logger::Level::Info);

		std::string directory{ "assets/shaders" };

		s_Shaders.emplace(MaterialType::Color, std::make_shared<Shader>(directory + "/DefaultShader.vert", directory + "/ColorMaterial.frag"));

		s_IsInitialized = true;

		Logger::Log("material shaders initialized!", Logger::Level::Info);
	}

	std::shared_ptr<Shader> MaterialShaderManager::Get(MaterialType type)
	{
		assert(s_IsInitialized);

		switch (type)
		{
			case MaterialType::Color:
				return s_Shaders.at(type);
		}

		return nullptr;
	}

	void MaterialShaderManager::Reload(MaterialType type)
	{
		assert(s_IsInitialized);

		s_Shaders.at(type)->Load();
	}
}