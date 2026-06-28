#pragma once

#include "../Shader.h"
#include "MaterialType.h"

namespace ultraviolet
{
	class Material
	{
	public:
		MaterialType Type;

	public:
		Material(MaterialType type);
		virtual ~Material() = default;

		virtual void Apply() const = 0;
		virtual void Free() = 0;
		virtual void AddToImGui(const std::string& id) = 0;

	protected:
		std::shared_ptr<Shader> m_Shader;
	};
}