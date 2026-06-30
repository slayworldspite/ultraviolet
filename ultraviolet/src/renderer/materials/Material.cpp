#include "Material.h"
#include "MaterialShaderManager.h"

namespace ultraviolet
{
	Material::Material(MaterialType type) :
		Shader{ MaterialShaderManager::Get(type) }
	{

	}
}