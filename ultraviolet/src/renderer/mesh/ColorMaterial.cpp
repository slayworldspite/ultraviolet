#include "ColorMaterial.h"

namespace ultraviolet
{
	ColorMaterial::ColorMaterial(const glm::vec4& color) :
		Color{ color }
	{

	}

	void ColorMaterial::Apply(const Shader& shader) const
	{
		shader.Use();
		shader.Set("u_Color", Color);
	}
}