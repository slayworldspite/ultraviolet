#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	class IMaterial
	{
	public:
		virtual ~IMaterial() = default;

		virtual void Apply(const Shader& shader) const = 0;
	};
}