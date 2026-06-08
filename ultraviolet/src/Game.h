#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"

namespace ultraviolet
{
	class Game : public GameWindow
	{
	public:
		Game();

	private:
		Renderer m_Renderer;
	};
}