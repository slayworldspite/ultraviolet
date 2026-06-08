#include "Game.h"

namespace ultraviolet
{
	Game::Game() :
		GameWindow{
			NativeWindowSettings{
				.Title{ "ultraviolet" },
				.Width{ 1280 },
				.Height{ 720 }
			},
			GameWindowSettings{
				.UpdateFrequency{ 60.0f }
			}
	},
		m_Renderer{ *this }
	{

	}

	void Game::OnStart()
	{

	}

	void Game::OnUpdate(double deltaTime)
	{

	}

	void Game::OnRender()
	{
		m_Renderer.BeginFrame();

		m_Renderer.EndFrame();
	}
}