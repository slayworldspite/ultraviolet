#include <imgui/imgui.h>
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
		m_ShowMenu{ false }
	{
		
	}

	void Game::OnStart()
	{
		PhysicsSystem::Initialize();

		SetRelativeMouseMode(true);

		m_ClearColor = glm::vec4{ 0.0f, 0.55f, 1.0f, 1.0f };
		m_Renderer.SetClearColor(m_ClearColor);

		m_Scene.AmbientLight.Color = glm::vec3{ 1.0f };
		m_Scene.AmbientLight.Intensity = 0.1f;

		m_Scene.DirectionalLight.Color = glm::vec3{ 1.0f };
		m_Scene.DirectionalLight.Pitch = -22.5f;
		m_Scene.DirectionalLight.Yaw = 22.5f;

		LoadOverlay();

		auto worldID{ PhysicsSystem::GetWorldID() };

		// capsule
		::b3BodyDef capsuleBodyDefinition{ ::b3DefaultBodyDef() };
		capsuleBodyDefinition.position = { -5.0f, 0.0f, 0.0f };
		capsuleBodyDefinition.type = ::b3_dynamicBody;
		capsuleBodyDefinition.motionLocks.angularX = true;
		capsuleBodyDefinition.motionLocks.angularZ = true;

		m_PlayerBody = std::make_unique<::b3BodyId>(::b3CreateBody(worldID, &capsuleBodyDefinition));

		m_PlayerCapsule = std::make_unique<::b3Capsule>();
		m_PlayerCapsule->center1 = { 0.0f, 0.5f, 0.0f };
		m_PlayerCapsule->center2 = { 0.0f, 1.5f, 0.0f };
		m_PlayerCapsule->radius = 0.4f;

		::b3ShapeDef capsuleShapeDefinition{ ::b3DefaultShapeDef() };
		capsuleShapeDefinition.density = 1.0f;
		capsuleShapeDefinition.baseMaterial.friction = 5.0f;

		::b3CreateCapsuleShape(*m_PlayerBody, &capsuleShapeDefinition, m_PlayerCapsule.get());

		// ground
		::b3BodyDef groundBodyDefinition{ ::b3DefaultBodyDef() };
		groundBodyDefinition.position = { 0.0f, 0.0f, 0.0f };

		m_Ground = std::make_unique<::b3BodyId>(::b3CreateBody(worldID, &groundBodyDefinition));

		::b3BoxHull groundBox{ ::b3MakeBoxHull(15.0f, 0.25f, 15.0f) };

		::b3ShapeDef groundShapeDefinition{ ::b3DefaultShapeDef() };
		::b3CreateHullShape(*m_Ground, &groundShapeDefinition, &groundBox.base);

		std::shared_ptr<TextureMaterial> brickMaterial{
			std::make_shared<TextureMaterial>(
				"assets/textures/brick_diffuse.jpg",
				"assets/textures/brick_specular.jpg",
				"assets/textures/brick_normal.png",
				32,
				15.0f
			)
		};

		std::shared_ptr<TextureMaterial> teapotMaterial{
			std::make_shared<TextureMaterial>(
				"assets/textures/default.png",
				"assets/textures/default.png",
				"assets/textures/default_normal.png",
				32,
				5.0f
			)
		};

		Model boxModel2{ Primitives::GetCube("ground", brickMaterial) };
		boxModel2.Scale = glm::vec3{ 15.0f, 0.25f, 15.0f };

		m_Scene.Models.push_back(std::move(boxModel2));

		auto teapot{ 
			EntityLoader::Load(
				"teapot", 
				PhysicsBodyType::Dynamic, 
				"assets/models/teapot/teapot.obj", 
				"assets/models/teapot/teapotCollision.obj", 
				0.35f
			) 
		};

		if (teapot)
		{
			for (Mesh& mesh : teapot->RenderObject.Meshes)
				mesh.Material = teapotMaterial;

			m_Scene.Entities.push_back(std::move(*teapot));
		}

		auto suzanne{
			EntityLoader::Load(
				"suzanne",
				PhysicsBodyType::Dynamic,
				"assets/models/suzanne/suzanne.obj",
				"assets/models/suzanne/suzanne_collision.obj"
			)
		};

		if (suzanne)
		{
			for (Mesh& mesh : suzanne->RenderObject.Meshes)
				mesh.Material = teapotMaterial;

			m_Scene.Entities.push_back(std::move(*suzanne));
		}
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;
		m_DeltaTime = deltaTime;
		m_Player.OnGround = false;

		m_Scene.Camera.Position = m_Player.Position;
		m_Scene.Camera.Position.y += m_Player.Height;

		PhysicsSystem::Update(deltaTime);

		auto playerPosition{ ::b3Body_GetPosition(*m_PlayerBody) };
		auto playerRotation{ ::b3Body_GetRotation(*m_PlayerBody) };

		m_Player.Position = glm::vec3{ playerPosition.x, playerPosition.y, playerPosition.z };

		m_Scene.Camera.Update();
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

			case Keycode::F:
				m_Player.Flying = !m_Player.Flying;
				break;

			case Keycode::SPACE:
				::b3Body_ApplyLinearImpulseToCenter(*m_PlayerBody, ::b3Vec3{ 0.0f, 5.0f, 0.0f }, true);
				break;

			case Keycode::F11:
				m_Fullscreen = !m_Fullscreen;
				SetFullscreen(m_Fullscreen);
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

		glm::vec3 forward{
			glm::cos(glm::radians(m_Scene.Camera.Yaw)),
			0.0f,
			glm::sin(glm::radians(m_Scene.Camera.Yaw))
		};

		glm::vec3 right{
			-glm::sin(glm::radians(m_Scene.Camera.Yaw)),
			0.0f,
			glm::cos(glm::radians(m_Scene.Camera.Yaw))
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

		if (glm::length(movementVector) > 0.0f)
			movementVector = glm::normalize(movementVector);

		::b3Vec3 velocity{ ::b3Body_GetLinearVelocity(*m_PlayerBody) };

		velocity.x += movementVector.x * m_Player.Speed * static_cast<float>(deltaTime);
		velocity.z += movementVector.z * m_Player.Speed * static_cast<float>(deltaTime);

		float horizontalSpeed{ std::sqrt(
			velocity.x * velocity.x +
			velocity.z * velocity.z
		) };

		if (horizontalSpeed > m_Player.MaxSpeed)
		{
			float scale{ m_Player.MaxSpeed / horizontalSpeed };

			velocity.x *= scale;
			velocity.z *= scale;
		}

		::b3Body_SetLinearVelocity(*m_PlayerBody, velocity);
	}

	void Game::DebugMenu()
	{
		::ImGui::Text("FPS: %f", 1.0 / m_DeltaTime);
		::ImGui::Text("Frametime: %.2fms", m_DeltaTime * 1000);

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

		m_Player.AddToImGui();

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