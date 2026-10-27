#include <unordered_map>

#include "MaterialShaderManager.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	bool s_IsInitialized{ false };
	std::unordered_map<MaterialType, Shader> s_Shaders{};

	void MaterialShaderManager::Initialize()
	{
		Logger::Log("initialzing material shaders...", Logger::Level::Info);

		std::filesystem::path directory{ "assets/shaders/materials" };
		std::filesystem::path defaultShaderPath{ directory / "DefaultShader.vert" };

		s_Shaders.emplace(MaterialType::Color, Shader{ defaultShaderPath, directory / "ColorMaterialDiffuse.frag" });
		s_Shaders.emplace(MaterialType::Texture, Shader{ defaultShaderPath, directory / "TextureMaterialDiffuse.frag" });
		s_Shaders.emplace(MaterialType::Light, Shader{ defaultShaderPath, directory / "LightMaterialDiffuse.frag" });

		s_IsInitialized = true;

		Logger::Log("material shaders initialized!", Logger::Level::Info);
	}

	Shader* MaterialShaderManager::Get(MaterialType type)
	{
		assert(s_IsInitialized);

		return &s_Shaders.at(type);
	}

	void MaterialShaderManager::Reload(MaterialType type)
	{
		assert(s_IsInitialized);

		s_Shaders.at(type).Load();
	}

	void MaterialShaderManager::ReloadAll()
	{
		assert(s_IsInitialized);

		for (size_t i{}; i < static_cast<size_t>(MaterialType::Count); i++)
			Reload(static_cast<MaterialType>(i));
	}
}