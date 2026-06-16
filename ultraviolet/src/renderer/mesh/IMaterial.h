#pragma once

#include "../Shader.h"

namespace ultraviolet
{
	class IMaterial
	{
	public:
		virtual ~IMaterial() = default;

		virtual void Apply(const Shader& shader) const = 0;
		virtual void Free() = 0;
		virtual void AddToImGui(const std::string& id, size_t index) = 0;
	};
}