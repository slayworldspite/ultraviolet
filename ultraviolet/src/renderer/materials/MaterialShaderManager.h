#pragma once

#include "../Shader.h"
#include "MaterialType.h"

namespace ultraviolet
{
	class MaterialShaderManager
	{
	public:
		MaterialShaderManager() = delete;

		static void Initialize();
		static std::shared_ptr<Shader> Get(MaterialType type);
		static void Reload(MaterialType type);
		static void ReloadAll();
	};
}