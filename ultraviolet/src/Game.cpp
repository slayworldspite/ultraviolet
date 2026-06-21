#include <format>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

#include "Game.h"
#include "input/Input.h"
#include "utils/Logger.h"
#include "renderer/mesh/ColorMaterial.h"
#include "renderer/model/ModelLoader.h"

namespace ultraviolet
{
	Game::Game() :
		GameWindow{
			NativeWindowSettings{
				.Title{ "ultraviolet" },
				.Width{ 1280 },
				.Height{ 720 }
			}
	},
		m_Renderer{ *this },
		m_Shader{ "assets/shaders/shader.vert", "assets/shaders/shader.frag" },
		m_DeltaTime{}
	{
		SetRelativeMouseMode(true);
	}

	void Game::OnStart()
	{
		m_Renderer.SetClearColor({ 0.05f, 0.15f, 0.15f, 1.0f });

		m_Camera.Position = { 0.0f, 1.0f, 3.0f };

		LoadAssets();
	}

	void Game::OnUpdate(double deltaTime)
	{
		m_DeltaTime = deltaTime;

		m_Camera.Update();

		ProcessInput(deltaTime);
	}

	void Game::OnRender()
	{
		m_Renderer.BeginFrame();

		m_Shader.Set("u_DefaultMetallic", m_DefaultMetallic);

		m_Camera.Apply(m_Shader, GetWindowSize());

		m_Renderer.Draw(m_Scene, m_Shader);

		DebugMenu();

		m_Renderer.EndFrame();
	}

	void Game::OnKeyPress(Keycode key)
	{
		switch (key)
		{
			case Keycode::ESCAPE:
				SetShouldClose(true);
				break;

			case Keycode::BACKSPACE:
				SetRelativeMouseMode(!GetRelativeMouseMode());
				break;

			case Keycode::O:
				m_Renderer.ToggleWireframeMode();
				break;

			case Keycode::R:
				LoadAssets();
				break;

			case Keycode::PLUS:
				static bool vsync{ true };
				vsync = !vsync;
				m_Renderer.SetVSync(vsync);
				break;
		}
	}

	void Game::OnMouseMove(float x, float y)
	{
		if (!GetRelativeMouseMode())
			return;

		float sensitivity{ 0.25f };

		m_Camera.Yaw += x * sensitivity;
		
		if (m_Camera.Yaw >= 360.0f)
			m_Camera.Yaw -= 360.0f;

		if (m_Camera.Yaw < 0)
			m_Camera.Yaw += 360.0f;

		m_Camera.Pitch -= y * sensitivity;

		if (m_Camera.Pitch < -89.0f)
			m_Camera.Pitch = -89.0f;

		if (m_Camera.Pitch > 89.0f)
			m_Camera.Pitch = 89.0f;
	}

	void Game::OnResize(int width, int height)
	{
		m_Renderer.SetViewport(width, height);
	}

	void Game::ProcessInput(double deltaTime)
	{
		if (::ImGui::GetIO().WantCaptureKeyboard)
			return;

		float movementSpeed{ 2.5f };

		glm::vec3 movementVector{};

		if (Input::IsKeyHeld(Scancode::W))
			movementVector.z += 1.0f;

		if (Input::IsKeyHeld(Scancode::S))
			movementVector.z -= 1.0f;

		if (Input::IsKeyHeld(Scancode::A))
			movementVector.x -= 1.0f;

		if (Input::IsKeyHeld(Scancode::D))
			movementVector.x += 1.0f;

		if (Input::IsKeyHeld(Scancode::SPACE))
			movementVector.y += 1.0f;

		if (Input::IsKeyHeld(Scancode::LCTRL))
			movementVector.y -= 1.0f;

		if (glm::length(movementVector) > 0.0f)
			movementVector = glm::normalize(movementVector);

		movementVector *= movementSpeed;

		m_Camera.Move(movementVector * static_cast<float>(deltaTime));
	}

	void Game::LoadAssets()
	{
		m_Scene.Models.clear();
	}

	void Game::DebugMenu()
	{
		::ImGui::Text("FPS: %f", 1.0 / m_DeltaTime);

		if (::ImGui::Button("Reload Shaders"))
			m_Shader.Load();

		::ImGui::DragFloat("Default Metallic", &m_DefaultMetallic, 0.01f, 0.0f, 1.0f);

		m_Camera.AddToImGui();
	}
}