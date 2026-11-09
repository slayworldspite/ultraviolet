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
		std::filesystem::path defaultVertexShaderPath{ directory / "DefaultShader.vert" };

		s_Shaders.emplace(MaterialType::Color, Shader{ defaultVertexShaderPath, directory / "ColorMaterial.frag" });
		s_Shaders.emplace(MaterialType::Texture, Shader{ defaultVertexShaderPath, directory / "TextureMaterial.frag" });
		s_Shaders.emplace(MaterialType::Light, Shader{ defaultVertexShaderPath, directory / "LightMaterial.frag" });

		s_IsInitialized = true;

		Logger::Log("material shaders initialized!", Logger::Level::Info);
	}

	Shader* MaterialShaderManager::Get(MaterialType materialType)
	{
		assert(s_IsInitialized);

		return &s_Shaders.at(materialType);
	}

	void MaterialShaderManager::Reload()
	{
		assert(s_IsInitialized);

		for (size_t materialIndex{}; materialIndex < static_cast<size_t>(MaterialType::Count); materialIndex++)
			Reload(static_cast<MaterialType>(materialIndex));
	}

	void MaterialShaderManager::Reload(MaterialType materialType)
	{
		assert(s_IsInitialized);

		s_Shaders.at(materialType).Load();
	}
}