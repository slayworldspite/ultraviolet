#pragma once

#include "../../renderer/scene/Scene.h"

namespace ultraviolet
{
	class Level
	{
	public:
		std::string Name;

	public:
		Level(std::string_view name);
		virtual ~Level() = default;

		virtual void Load(Scene& scene) = 0;
	};
}