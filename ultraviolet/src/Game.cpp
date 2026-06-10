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
		m_Renderer{ *this },
		m_Shader{ "assets/shaders/Test.vert", "assets/shaders/Test.frag" },
		m_Mesh{ std::vector<Vertex>{ Vertex{ .Position{ 0.5f, -0.5f, 0.0f } }, Vertex{ .Position{ -0.5f, -0.5f, 0.0f } }, Vertex{ .Position{ 0.0f, 0.5f, 0.0f } } } }
	{

	}


	void Game::OnStart()
	{
		m_Renderer.SetClearColor({ 0.05f, 0.45f, 1.0f, 1.0f });
	}

	void Game::OnUpdate(double deltaTime)
	{

	}

	void Game::OnRender()
	{
		m_Renderer.BeginFrame();

		m_Shader.Use();
		m_Shader.Set("u_Color", { 0.85f, 0.15f, 0.95f, 1.0f });
		m_Renderer.Draw(m_Mesh, m_Shader);

		m_Renderer.EndFrame();
	}
}