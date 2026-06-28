#include "Material.h"
#include "MaterialShaderManager.h"

namespace ultraviolet
{
	Material::Material(MaterialType type) :
		m_Shader{ MaterialShaderManager::Get(type) }
	{

	}
}