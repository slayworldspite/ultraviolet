#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	class IMaterial
	{
	public:
		virtual void Apply(const Shader& shader) const = 0;
	};
}