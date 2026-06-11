#include <format>

#include <imgui/imgui.h>
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
		m_Model{ ModelLoader::Load("assets/models/teapot/teapot.obj") }
	{
		SetRelativeMouseMode(true);
	}

	void Game::OnStart()
	{
		m_Renderer.SetClearColor({ 0.15f, 0.15f, 0.2f, 1.0f });
		m_Camera.Position = { 0.0f, 0.0f, 3.0f };

		m_Model.Scale = { 0.1f, 0.1f, 0.1f };
	}

	void Game::OnUpdate(double deltaTime)
	{
		m_Camera.Update();
		ProcessInput(deltaTime);
	}

	void Game::OnRender()
	{
		m_Renderer.BeginFrame();

		m_Camera.Apply(m_Shader, GetWindowSize());
		
		m_Renderer.Draw(m_Model, m_Shader);

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
		if (ImGui::GetIO().WantCaptureKeyboard)
			return;

		float movementSpeed{ 100.0f * static_cast<float>(deltaTime) };

		glm::vec3 movementVector{};

		if (Input::IsKeyHeld(Scancode::W))
			movementVector.z += movementSpeed;

		if (Input::IsKeyHeld(Scancode::S))
			movementVector.z -= movementSpeed;

		if (Input::IsKeyHeld(Scancode::A))
			movementVector.x -= movementSpeed;

		if (Input::IsKeyHeld(Scancode::D))
			movementVector.x += movementSpeed;

		if (Input::IsKeyHeld(Scancode::SPACE))
			movementVector.y += movementSpeed;

		if (Input::IsKeyHeld(Scancode::LCTRL))
			movementVector.y -= movementSpeed;

		m_Camera.Move(movementVector * static_cast<float>(deltaTime));
	}
}