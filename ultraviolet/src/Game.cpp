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
		m_DeltaTime{}
	{
		SetRelativeMouseMode(true);
	}

	void Game::OnStart()
	{
		m_Renderer.SetClearColor({ 0.15f, 0.15f, 0.2f, 1.0f });
		m_Camera.Position = { 0.0f, 0.0f, 3.0f };

		m_Shader.Set("u_Texture", 0);

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

		m_Camera.Apply(m_Shader, GetWindowSize());
		
		for (const Model& model : m_Models)
		{
			m_Renderer.Draw(model, m_Shader);
		}

		::ImGui::Text("%f", 1.0 / m_DeltaTime);
		::ImGui::DragFloat3("camera pos", glm::value_ptr(m_Camera.Position));

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

		float movementSpeed{ 5.0f };

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
		m_Models.clear();

		//Model dragon{ ModelLoader::Load("assets/models/dragon/dragon.obj") };
		//Model teapot{ ModelLoader::Load("assets/models/teapot/teapot.obj") };
		Model backpack{ ModelLoader::Load("assets/models/backpack/backpack.fbx") };
		Model test{ ModelLoader::Load("assets/models/test/test.obj") };

		//dragon.Position.x -= 5;
		//teapot.Position.x += 5;
		//teapot.Scale /= 50;
		backpack.Position.z += 10;

		//m_Models.push_back(std::move(dragon));
		//m_Models.push_back(std::move(teapot));
		m_Models.push_back(std::move(backpack));
		m_Models.push_back(std::move(test));
	}
}