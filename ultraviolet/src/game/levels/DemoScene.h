#pragma once

#include "Level.h"

namespace ultraviolet
{
	class DemoScene : public Level
	{
	public:
		DemoScene();

		virtual void Load(Scene& scene) override;
	};
}