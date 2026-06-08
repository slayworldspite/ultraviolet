#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"

namespace ultraviolet
{
	class Game : public GameWindow
	{
	public:
		Game();

	protected:
		void OnStart() override;
		void OnUpdate(double deltaTime) override;
		void OnRender() override;

	private:
		Renderer m_Renderer;
	};
}