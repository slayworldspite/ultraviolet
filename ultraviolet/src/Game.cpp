#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <tracy/tracy/Tracy.hpp>

#include "Game.h"
#include "input/Input.h"
#include "utils/Logger.h"
#include "renderer/materials/ColorMaterial.h"
#include "renderer/materials/TextureMaterial.h"
#include "renderer/materials/OverlayMaterial.h"
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

		m_ClearColor = glm::vec4{ 0.0f, 0.55f, 1.0f, 1.0f };

		m_Renderer.SetClearColor(m_ClearColor);

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

		m_ShowMenu = true;
		m_FirstFrame = true;

		LoadScene();
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;
		m_DeltaTime = deltaTime;
		m_Player.OnGround = false;

		if (m_FirstFrame)
		{
			deltaTime = 0.0;
			m_DeltaTime = 0.0;
			m_FirstFrame = false;
		}

		m_Player.Update(deltaTime);

		m_Camera.Position = m_Player.Position;
		m_Camera.Position.y += m_Player.Height;

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

		Model* playerModel{ m_Scene.GetModel("playerModel") };

		if (playerModel)
		{
			playerModel->Position = m_Player.Position;
			playerModel->Position.y += m_Player.Height;

			for (const Model& model : m_Scene.Models)
			{
				if (model.ID == playerModel->ID)
					continue;

				if (playerModel->Intersects(model))
				{
					AABB playerBoundingBox{ playerModel->GetWorldAABB() };
					AABB modelBoundingBox{ model.GetWorldAABB() };

					float x = glm::min(
						playerBoundingBox.Max.x - modelBoundingBox.Min.x,
						modelBoundingBox.Max.x - playerBoundingBox.Min.x
					);

					float y = glm::min(
						playerBoundingBox.Max.y - modelBoundingBox.Min.y,
						modelBoundingBox.Max.y - playerBoundingBox.Min.y
					);

					float z = glm::min(
						playerBoundingBox.Max.z - modelBoundingBox.Min.z,
						modelBoundingBox.Max.z - playerBoundingBox.Min.z
					);

					if (x < y && x < z)
						m_Player.Position.x += x * glm::sign(m_Player.Position.x - model.Position.x);

					else if (y < z)
					{
						if (playerBoundingBox.Min.y < modelBoundingBox.Max.y)
						{
							m_Player.Position.y += y;
							m_Player.Velocity.y = 0.0f;
							m_Player.OnGround = true;
						}
						else
						{
							m_Player.Position.y -= y;
							m_Player.Velocity.y = 0.0f;
						}
					}
					else
					{
						m_Player.Position.z += z * glm::sign(m_Player.Position.z - model.Position.z);
					}
				}
			}
		}

		ProcessInput(deltaTime);
	}

	void Game::OnRender()
	{
		ZoneScoped;
		m_Renderer.BeginFrame();

		m_Camera.Apply(GetWindowSize());
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

			case Keycode::R:
				LoadScene();
				break;

			case Keycode::B:
				m_ShowMenu = !m_ShowMenu;
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

		glm::vec3 forward{
			glm::cos(glm::radians(m_Camera.Yaw)),
			0.0f,
			glm::sin(glm::radians(m_Camera.Yaw))
		};

		glm::vec3 right{
			-glm::sin(glm::radians(m_Camera.Yaw)),
			0.0f,
			glm::cos(glm::radians(m_Camera.Yaw))
		};

		glm::vec3 movementVector{};

		if (Input::IsKeyHeld(Scancode::W))
			movementVector += forward;

		if (Input::IsKeyHeld(Scancode::S))
			movementVector -= forward;

		if (Input::IsKeyHeld(Scancode::A))
			movementVector -= right;

		if (Input::IsKeyHeld(Scancode::D))
			movementVector += right;

		if (Input::IsKeyHeld(Scancode::SPACE))
		{
			if (m_Player.OnGround || m_Player.Flying)
				m_Player.Velocity.y += m_Player.JumpHeight;
		}

		if (Input::IsKeyHeld(Scancode::LCTRL))
		{
			if (m_Player.Flying)
				m_Player.Velocity.y -= m_Player.JumpHeight;
		}

		if (glm::length(movementVector) > 0.0f)
			movementVector = glm::normalize(movementVector);

		m_Player.Velocity.x += movementVector.x * m_Player.Speed * static_cast<float>(deltaTime);
		m_Player.Velocity.z += movementVector.z * m_Player.Speed * static_cast<float>(deltaTime);
	}

	void Game::LoadScene()
	{
		m_Scene.Clear();

		m_Player.Position = { -5.0f, 0.0f, 0.0f };
		m_Camera.Pitch = -45.0f;

		m_Scene.AmbientLight.Color = { 1.0f, 1.0f, 1.0f };
		m_Scene.AmbientLight.Intensity = 0.2f;

		m_Scene.DirectionalLight.Enabled = true;
		m_Scene.DirectionalLight.Pitch = -25.0f;
		m_Scene.DirectionalLight.Yaw = -75.0f;
		m_Scene.DirectionalLight.Intensity = 1.0f;

		PointLight pointLight{};
		pointLight.ID = "light";
		pointLight.Position = glm::vec3{ 1.5f, 0.5f, 0.5f };
		pointLight.Color = glm::vec3{ 0.0f, 0.5f, 1.0f };
		pointLight.Intensity = 0.7f;

		m_Scene.PointLights.push_back(std::move(pointLight));

		m_Scene.Models.clear();

		Model ground{ Primitives::GetCube(
			"ground",
			std::make_shared<TextureMaterial>(
				"assets/textures/brick_diffuse.jpg",
				"assets/textures/brick_specular.jpg",
				"assets/textures/brick_normal.png",
				100
			)
		) };

		ground.Scale.x *= 10.0f;
		ground.Scale.y = 0.01f;
		ground.Scale.z *= 10.0f;

		ground.Position.y -= ground.Scale.y;

		m_Scene.Models.push_back(std::move(ground));

		Model sphere1{ Primitives::GetSphere(
			"sphere1",
			32,
			32,
			std::make_shared<ColorMaterial>(
				glm::vec3{ 0.0f, 1.0f, 0.5f },
				256
			)
		) };
		sphere1.Position = { 0.5f, 0.5f, -1.5f };
		sphere1.Scale = { 0.5f, 0.5f, 0.5f };

		m_Scene.Models.push_back(std::move(sphere1));

		Model sphere2{ Primitives::GetSphere(
			"sphere2",
			32,
			32,
			std::make_shared<ColorMaterial>(
				glm::vec3{ 0.65f, 0.95f, 1.0f },
				32
			)
		) };
		sphere2.Position = { 4.0f, 2.5f, -2.0f };
		sphere2.Scale = glm::vec3{ 2.5f };

		m_Scene.Models.push_back(std::move(sphere2));

		Model cube{ Primitives::GetCube("cube", std::make_shared<TextureMaterial>(
			"assets/textures/wood_diffuse.jpg",
			"assets/textures/wood_specular.jpg",
			"assets/textures/wood_normal.jpg",
			100
		)) };

		cube.Position = { 5.0f, 3.0f, 5.0f };
		cube.Scale = glm::vec3{ 3.0f };

		m_Scene.Models.push_back(std::move(cube));

		Model playerModel{ Primitives::GetCube("playerModel", nullptr) };
		playerModel.Scale.x /= 2.5f;
		playerModel.Scale.y = 1.8f;
		playerModel.Scale.z /= 2.5f;
		playerModel.Visible = false;

		m_Scene.Models.push_back(std::move(playerModel));

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

			if (::ImGui::Button("Reload All Material Shaders"))
				MaterialShaderManager::ReloadAll();

			::ImGui::ColorPicker4("Sky Color", glm::value_ptr(m_ClearColor));

			if (::ImGui::Button("Set Sky Color"))
				m_Renderer.SetClearColor(m_ClearColor);

			::ImGui::Spacing();

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

		m_Player.AddToImGui();

		m_Camera.AddToImGui();

		m_Scene.AddToImGui();
	}
}