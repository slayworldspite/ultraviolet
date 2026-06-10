#include "ColorMaterial.h"

namespace ultraviolet
{
	ColorMaterial::ColorMaterial(glm::vec4 color) :
		Color{ color }
	{

	}

	ColorMaterial::ColorMaterial(float r, float g, float b, float a) :
		Color{ glm::vec4{r, g, b, a} }
	{

	}

	void ColorMaterial::Apply(const Shader& shader) const
	{
		shader.Set("u_Color", Color);
	}
}