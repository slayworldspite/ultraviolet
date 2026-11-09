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
		static Shader* Get(MaterialType materialType);
		static void Reload();
		static void Reload(MaterialType materialType);
	};
}