#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <tracy/tracy/Tracy.hpp>
#include <box3d/box3d.h>

#include "Game.h"
#include "input/Input.h"
#include "utils/Logger.h"
#include "renderer/materials/ColorMaterial.h"
#include "renderer/materials/TextureMaterial.h"
#include "renderer/materials/OverlayMaterial.h"
#include "renderer/materials/MaterialShaderManager.h"
#include "renderer/model/ModelLoader.h"
#include "renderer/model/Primitives.h"
#include "physics/PhysicsSystem.h"
#include "physics/EntityLoader.h"
#include "game/levels/Levels.h"

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
		m_DeltaTime{},
		m_Fullscreen{ false },
		m_ShowMenu{ false },
		m_FreeCam{ true }
	{
		
	}

	void Game::OnStart()
	{
		SetRelativeMouseMode(true);

		m_ClearColor = glm::vec4{ 0.0f, 0.55f, 1.0f, 1.0f };
		m_Renderer.SetClearColor(m_ClearColor);

		m_Player = std::make_unique<Player>();

		DemoScene{}.Load(m_Scene);

		LoadOverlay();
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;

		deltaTime = std::min(deltaTime, 0.05);

		m_DeltaTime = deltaTime;

		PhysicsSystem::Update(deltaTime);

		ProjectionCamera& camera{ m_Scene.Camera };

		m_Player->Update(deltaTime);

		camera.Update();
		m_Scene.Update();

		ProcessInput(deltaTime);
	}

	void Game::OnRender()
	{
		ZoneScoped;
		m_Renderer.BeginFrame();

		m_Scene.Camera.Apply(GetWindowSize());
		m_Renderer.Draw(m_Scene);

		if (m_ShowMenu)
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

			case Keycode::B:
				m_ShowMenu = !m_ShowMenu;
				break;

			case Keycode::F11:
				m_Fullscreen = !m_Fullscreen;
				SetFullscreen(m_Fullscreen);
				break;

			case Keycode::N:
				m_FreeCam = !m_FreeCam;
				break;
		}
	}

	void Game::OnMouseMove(float x, float y)
	{
		if (!GetRelativeMouseMode())
			return;

		float sensitivity{ 0.25f };

		m_Scene.Camera.Yaw += x * sensitivity;
		
		if (m_Scene.Camera.Yaw >= 360.0f)
			m_Scene.Camera.Yaw -= 360.0f;

		if (m_Scene.Camera.Yaw < 0)
			m_Scene.Camera.Yaw += 360.0f;

		m_Scene.Camera.Pitch -= y * sensitivity;

		if (m_Scene.Camera.Pitch < -89.0f)
			m_Scene.Camera.Pitch = -89.0f;

		if (m_Scene.Camera.Pitch > 89.0f)
			m_Scene.Camera.Pitch = 89.0f;
	}

	void Game::OnResize(int width, int height)
	{
		m_Renderer.Resize(width, height);
	}

	void Game::ProcessInput(double deltaTime)
	{
		if (::ImGui::GetIO().WantCaptureKeyboard)
			return;

		static float speed{ 3.5f };
		float fDeltaTime{ static_cast<float>(deltaTime) };

		glm::vec3 throttle{};

		if (Input::IsKeyHeld(Scancode::W))
			throttle.x += 1.0f;

		if (Input::IsKeyHeld(Scancode::S))
			throttle.x -= 1.0f;

		if (Input::IsKeyHeld(Scancode::A))
			throttle.z -= 1.0f;

		if (Input::IsKeyHeld(Scancode::D))
			throttle.z += 1.0f;

		if (m_FreeCam)
		{
			ProjectionCamera& camera{ m_Scene.Camera };

			glm::vec3 forward{ camera.GetForward() };
			glm::vec3 right{ glm::normalize(glm::cross(camera.GetForward(), camera.GetWorldUp())) };

			if (Input::IsKeyHeld(Scancode::SPACE))
				camera.Position.y += speed * fDeltaTime;

			if (Input::IsKeyHeld(Scancode::LCTRL))
				camera.Position.y -= speed * fDeltaTime;

			glm::vec3 movement{};
			movement += throttle.x * forward;
			movement += throttle.z * right;

			camera.Position += movement * fDeltaTime * speed;
		}
		else
		{
			m_Player->Throttle = throttle * static_cast<float>(deltaTime);
		}
	}

	void Game::DebugMenu()
	{
		::ImGui::Text("FPS: %f", 1.0 / m_DeltaTime);
		::ImGui::Text("Frame Time: %.2fms", m_DeltaTime * 1000);

		static std::vector<double> frameTimes{};
		static std::vector<double> frameTimeHistory{};
		static double time{};
		static double maxFrameTime{};

		time += m_DeltaTime;

		frameTimes.push_back(time);
		frameTimeHistory.push_back(m_DeltaTime * 1000);

		maxFrameTime = std::max(maxFrameTime, m_DeltaTime);

		if (ImPlot::BeginPlot("Frame Time Graph", ImVec2(-1, ImGui::GetTextLineHeight() * 30)))
		{
			ImPlot::SetupAxes("Time", "Frame Time");
			ImPlot::SetupAxisLimits(ImAxis_X1, time - 10.0, time, ImGuiCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, maxFrameTime * 1000 + 2, ImGuiCond_Always);

			ImPlot::PlotLine(
				"Frame Time",
				frameTimes.data(),
				frameTimeHistory.data(),
				static_cast<int>(frameTimeHistory.size())
			);

			ImPlot::EndPlot();
		}

		static bool vsync{ true };
		::ImGui::Checkbox("V-Sync", &vsync);

		m_Renderer.SetVSync(vsync);

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

			if (::ImGui::Button("Reload All Material Shaders"))
				MaterialShaderManager::ReloadAll();

			::ImGui::ColorPicker4("Sky Color", glm::value_ptr(m_ClearColor));

			if (::ImGui::Button("Set Sky Color"))
				m_Renderer.SetClearColor(m_ClearColor);

			::ImGui::Spacing();

			m_Renderer.AddToImGui();

			::ImGui::Indent(-16.0f);
		}

		m_Player->AddToImGui();

		m_Scene.Camera.AddToImGui();

		m_Scene.AddToImGui();
	}

	void Game::LoadOverlay()
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(4);

		Vertex vertex{};
		vertex.Position = { -1.0f, 0.0f, 1.0f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, 1.0f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { -1.0f, 0.0f, -1.0f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, -1.0f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		std::shared_ptr<Texture> overlay{ std::make_shared<Texture>("assets/textures/overlay.png", TextureType::Overlay) };
		m_Renderer.SetOverlay(overlay);
	}
}