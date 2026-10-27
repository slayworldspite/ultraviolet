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
		static Shader* Get(MaterialType type);
		static void Reload(MaterialType type);
		static void ReloadAll();
	};
}