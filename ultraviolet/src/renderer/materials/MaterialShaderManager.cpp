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

		std::string directory{ "assets/shaders/materials/" };

		s_Shaders.emplace(MaterialType::Color, std::make_shared<Shader>(directory + "DefaultShader.vert", directory + "ColorMaterial.frag"));
		s_Shaders.emplace(MaterialType::Texture, std::make_shared<Shader>(directory + "DefaultShader.vert", directory + "TextureMaterial.frag"));
		s_Shaders.emplace(MaterialType::Light, std::make_shared<Shader>(directory + "DefaultShader.vert", directory + "LightMaterial.frag"));

		s_IsInitialized = true;

		Logger::Log("material shaders initialized!", Logger::Level::Info);
	}

	std::shared_ptr<Shader> MaterialShaderManager::Get(MaterialType type)
	{
		assert(s_IsInitialized);

		return s_Shaders.at(type);
	}

	void MaterialShaderManager::Reload(MaterialType type)
	{
		assert(s_IsInitialized);

		s_Shaders.at(type)->Load();
	}

	void MaterialShaderManager::ReloadAll()
	{
		assert(s_IsInitialized);

		s_Shaders.at(MaterialType::Color)->Load();
		s_Shaders.at(MaterialType::Texture)->Load();
		s_Shaders.at(MaterialType::Light)->Load();
		s_Shaders.at(MaterialType::Overlay)->Load();
	}
}