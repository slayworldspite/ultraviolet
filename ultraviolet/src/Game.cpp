#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <tracy/tracy/Tracy.hpp>

#include "Game.h"
#include "input/Input.h"
#include "utils/Logger.h"
#include "renderer/materials/ColorMaterial.h"
#include "renderer/materials/TextureMaterial.h"
#include "renderer/materials/MaterialShaderManager.h"
#include "renderer/model/ModelLoader.h"
#include "renderer/model/Primitives.h"

namespace ultraviolet
{
	Game::Game() :
		GameWindow{
			NativeWindowSettings{
				.Title{ "ultraviolet" },
				.Width{ 1920 },
				.Height{ 1080 }
			}
	},
		m_Renderer{ *this },
		m_DeltaTime{}
	{
		
	}

	void Game::OnStart()
	{
		SetRelativeMouseMode(true);

		m_Renderer.SetClearColor({ 0.05f, 0.15f, 0.15f, 1.0f });

		LoadScene();
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;
		m_DeltaTime = deltaTime;

		m_Camera.Update();
		m_Scene.Update();

		ProcessInput(deltaTime);
	}

	void Game::OnRender()
	{
		ZoneScoped;
		m_Renderer.BeginFrame();

		m_Camera.Apply(GetWindowSize());
		m_Renderer.Draw(m_Scene);

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

			case Keycode::R:
				LoadScene();
				break;

			case Keycode::RIGHTBRACKET:
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

	void Game::LoadScene()
	{
		m_Scene.Clear();

		m_Camera.Position = { 0.0f, 1.0f, 3.0f };
		m_Scene.AmbientLight.Color = { 1.0f, 1.0f, 1.0f };
		m_Scene.AmbientLight.Intensity = 0.10f;

		m_Scene.PointLights.emplace_back("light");
		m_Scene.GetPointLight("light")->Position += 2.0f;

		m_Scene.Models.clear();

		m_Scene.Models.push_back(std::move(
			Primitives::GetPlane(
				"plane", 
				std::make_shared<TextureMaterial>(
					"assets/textures/test_diff.png", 
					"assets/textures/test_spec.png",
					32
				)
			)
		));

		m_Scene.GetModel("plane")->Position.y -= 1.0f;
		m_Scene.GetModel("plane")->Scale.x *= 4.0f;
		m_Scene.GetModel("plane")->Scale.z *= 4.0f;

		m_Scene.Models.push_back(std::move(
			Primitives::GetSphere(
				"sphere",
				32,
				32,
				std::make_shared<ColorMaterial>(
					glm::vec3{ 1.0f },
					32
				)
			)
		));
	}

	void Game::DebugMenu()
	{
		::ImGui::Text("FPS: %f", 1.0 / m_DeltaTime);

		if (::ImGui::CollapsingHeader("Renderer"))
		{
			::ImGui::Indent(16.0f);

			static bool wireframeMode{ false };
			static bool lastWireframeMode{ wireframeMode };

			::ImGui::Checkbox("Wireframe Mode", &wireframeMode);

			if (wireframeMode != lastWireframeMode)
			{
				lastWireframeMode = wireframeMode;
				m_Renderer.SetWireframeMode(wireframeMode);
			}

			if (::ImGui::Button("Reload All Shaders"))
				MaterialShaderManager::ReloadAll();

			static int materialIndex{};

			::ImGui::InputInt("Index of Material", &materialIndex);

			if (::ImGui::Button("[RISKY] Reload Shader of Value"))
				MaterialShaderManager::Reload(static_cast<MaterialType>(materialIndex));

			::ImGui::Indent(-16.0f);
		}

		if (::ImGui::CollapsingHeader("Game"))
		{
			::ImGui::Indent(16.0f);

			if (::ImGui::Button("Reload Assets"))
				LoadScene();

			::ImGui::Indent(-16.0f);
		}

		m_Camera.AddToImGui();

		m_Scene.AddToImGui();
	}
}