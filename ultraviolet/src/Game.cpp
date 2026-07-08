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

		m_Renderer.SetClearColor({ 0.025f, 0.025f, 0.025f, 1.0f });

		m_Camera.Position = { 0.0f, 1.0f, 3.0f };

		LoadScene();
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;
		m_DeltaTime = deltaTime;

		m_Camera.Update();
		m_Scene.Update();

		Model* pointLightModel{ m_Scene.GetModel("plmesh") };
		PointLight* pointLight{ m_Scene.GetPointLight("light") };

		if (pointLightModel && pointLight)
		{
			pointLightModel->Position = pointLight->Position;
			m_PointLightMaterial->Color = pointLight->Color;
			m_PointLightMaterial->Intensity = pointLight->Intensity;
			m_PointLightMaterial->Enabled = pointLight->Enabled;
		}

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

		m_Scene.AmbientLight.Color = { 1.0f, 1.0f, 1.0f };
		m_Scene.AmbientLight.Intensity = 0.1f;

		m_Scene.DirectionalLight.Enabled = true;
		m_Scene.DirectionalLight.Pitch = -45.0f;
		m_Scene.DirectionalLight.Yaw = -45.0f;
		m_Scene.DirectionalLight.Intensity = 0.6f;

		PointLight pointLight{};
		pointLight.ID = "light";
		pointLight.Position = glm::vec3{ 0.6f, -0.8f, -0.4f };
		pointLight.Color = glm::vec3{ 0.0f, 0.5f, 1.0f };
		pointLight.Intensity = 1.0f;

		m_Scene.PointLights.push_back(std::move(pointLight));

		m_Scene.Models.clear();

		Model plane{ Primitives::GetPlane(
			"plane",
			std::make_shared<ColorMaterial>(
				glm::vec3{ 0.8f },
				32
			)
		) };

		plane.Position.y -= 1.0f;
		plane.Scale.x *= 4.0f;
		plane.Scale.z *= 4.0f;

		m_Scene.Models.push_back(std::move(plane));

		Model sphere1{ Primitives::GetSphere(
			"sphere1",
			32,
			32,
			std::make_shared<ColorMaterial>(
				glm::vec3{ 1.0f },
				32
			)
		) };
		sphere1.Position = { 0.5f, -0.5f, -1.5f };
		sphere1.Scale = { 0.5f, 0.5f, 0.5f };

		m_Scene.Models.push_back(std::move(sphere1));

		Model sphere2{ Primitives::GetSphere(
			"sphere2",
			32,
			32,
			std::make_shared<ColorMaterial>(
				glm::vec3{ 1.0f, 0.0f, 0.0f },
				32
			)
		) };
		sphere2.Position = { 2.0f, 0.0f, -2.0f };

		m_Scene.Models.push_back(std::move(sphere2));

		m_PointLightMaterial = std::make_shared<LightMaterial>(m_Scene.GetPointLight("light")->Color);

		Model plmesh{ Primitives::GetSphere("plmesh", 6, 8, m_PointLightMaterial) };
		plmesh.Scale /= 10.0f;
		plmesh.Position = pointLight.Position;
		plmesh.CastsShadow = false;

		m_Scene.Models.push_back(std::move(plmesh));
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

			m_Renderer.AddToImGui();

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