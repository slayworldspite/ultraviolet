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

	void Game::OnUpdate(double deltaTime)
	{
		static double time{};

		time += deltaTime;

		m_Renderer.SetClearColor({ sinf(time), sinf(time + 90), sinf(time + 180), 1.0f });
	}

	void Game::OnRender()
	{
		m_Renderer.BeginFrame();

		m_Renderer.EndFrame();
	}
}