#pragma once

#include "../Shader.h"
#include "MaterialType.h"

namespace ultraviolet
{
	class Material
	{
	public:
		std::shared_ptr<Shader> Shader;

	public:
		Material(MaterialType type);
		virtual ~Material() = default;

		virtual void Apply() const = 0;
		virtual void Free() = 0;
		virtual void AddToImGui(const std::string& id) = 0;
	};
}