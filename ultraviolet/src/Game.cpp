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
		m_ShowMenu{ false }
	{
		
	}

	void Game::OnStart()
	{
		PhysicsSystem::Initialize();

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

		m_Player.Flying = true;

		m_Scene.AmbientLight.Color = glm::vec3{ 1.0f };
		m_Scene.AmbientLight.Intensity = 0.2f;

		m_Scene.DirectionalLight.Color = glm::vec3{ 1.0f };
		m_Scene.DirectionalLight.Pitch = -22.5f;
		m_Scene.DirectionalLight.Yaw = 22.5f;

		m_Player.Position.x = -5.0f;

		auto worldID{ PhysicsSystem::GetWorldID() };

		// capsule
		::b3BodyDef capsuleBodyDefinition{ ::b3DefaultBodyDef() };
		capsuleBodyDefinition.position = { -5.0f, 15.0f, 0.0f };
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

		// box
		::b3BodyDef bodyDefinition{ ::b3DefaultBodyDef() };
		bodyDefinition.type = ::b3_dynamicBody;
		bodyDefinition.position = { 0.0f, 4.0f, 0.0f };
		bodyDefinition.rotation.v.x = 0.183974f;
		bodyDefinition.rotation.v.z = 0.5519221f;
		bodyDefinition.rotation.s = 0.8133483f;

		m_Box = std::make_unique<::b3BodyId>(::b3CreateBody(worldID, &bodyDefinition));
		
		::b3BoxHull dynamicBox{ ::b3MakeCubeHull(1.0f) };

		::b3ShapeDef boxShapeDefinition{ ::b3DefaultShapeDef() };
		boxShapeDefinition.density = 0.15f;
		boxShapeDefinition.baseMaterial.friction = 0.3f;

		::b3CreateHullShape(*m_Box, &boxShapeDefinition, &dynamicBox.base);

		// ramp
		::b3BodyDef rampBodyDefinition{ ::b3DefaultBodyDef() };
		rampBodyDefinition.position = { 0.0f, -0.25f, 0.0f };
		rampBodyDefinition.rotation.v.x = 0.1950903f;
		rampBodyDefinition.rotation.s = 0.9807853f;

		m_Ramp = std::make_unique<::b3BodyId>(::b3CreateBody(worldID, &rampBodyDefinition));

		::b3BoxHull rampBox{ ::b3MakeBoxHull(1.0f, 1.0f, 5.0f) };

		::b3ShapeDef rampShapeDefinition{ ::b3DefaultShapeDef() };
		::b3CreateHullShape(*m_Ramp, &rampShapeDefinition, &rampBox.base);

		// meshes
		std::shared_ptr<TextureMaterial> brickMaterial{
			std::make_shared<TextureMaterial>(
				"assets/textures/brick_diffuse.jpg",
				"assets/textures/brick_specular.jpg",
				"assets/textures/brick_normal.png",
				32
			)
		};
		std::shared_ptr<TextureMaterial> brickMaterial2{
			std::make_shared<TextureMaterial>(
				"assets/textures/brick_diffuse.jpg",
				"assets/textures/brick_specular.jpg",
				"assets/textures/brick_normal.png",
				32,
				15.0f
			)
		};

		Model boxModel{ Primitives::GetCube("box", brickMaterial) };
		Model boxModel2{ Primitives::GetCube("box2", brickMaterial2) };
		Model boxModel3{ Primitives::GetCube("ramp", brickMaterial) };

		boxModel.Scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
		boxModel.Position.y = 4.0f;

		boxModel2.Scale = glm::vec3{ 15.0f, 0.25f, 15.0f };

		boxModel3.Scale = glm::vec3{ 1.0f, 1.0f, 5.0f };
		boxModel3.Position = { 0.0f, -0.25f, 0.0f };
		boxModel3.Rotation.x = 22.5f;

		m_Scene.Models.push_back(std::move(boxModel));
		m_Scene.Models.push_back(std::move(boxModel2));
		m_Scene.Models.push_back(std::move(boxModel3));
		
		std::shared_ptr<Texture> overlay{ std::make_shared<Texture>("assets/textures/overlay.png", TextureType::Overlay) };
		m_Renderer.SetOverlay(overlay);
	}

	void Game::OnUpdate(double deltaTime)
	{
		ZoneScoped;
		m_DeltaTime = deltaTime;
		m_Player.OnGround = false;

		m_Scene.Camera.Position = m_Player.Position;
		m_Scene.Camera.Position.y += m_Player.Height;

		PhysicsSystem::Update(deltaTime);

		auto boxPosition{ ::b3Body_GetPosition(*m_Box) };
		auto boxRotation{ ::b3Body_GetRotation(*m_Box) };

		auto box{ m_Scene.GetModel("box") };

		if (box)
		{
			box->Position.x = boxPosition.x;
			box->Position.y = boxPosition.y;
			box->Position.z = boxPosition.z;

			glm::quat rotation{ boxRotation.s, boxRotation.v.x, boxRotation.v.y, boxRotation.v.z };	
			box->RotationQuaternion = rotation;
		}

		auto playerPosition{ ::b3Body_GetPosition(*m_PlayerBody) };
		auto playerRotation{ ::b3Body_GetRotation(*m_PlayerBody) };

		m_Player.Position = glm::vec3{ playerPosition.x, playerPosition.y, playerPosition.z };

		//m_Player.UpdateVelocity(deltaTime);
		//m_Player.ApplyVelocity(deltaTime);
		//m_Player.ApplyFriction(deltaTime);

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

			case Keycode::I:
				::b3Body_ApplyLinearImpulseToCenter(*m_Box, ::b3Vec3{ 0.0f, 25.0f, 0.0f }, true);
				break;

			case Keycode::O:
				::b3Body_ApplyAngularImpulse(*m_Box, ::b3Vec3{ 0.0f, 0.0f, 1.0f }, true);
				break;

			case Keycode::SPACE:
				::b3Body_ApplyLinearImpulseToCenter(*m_PlayerBody, ::b3Vec3{ 0.0f, 5.0f, 0.0f }, true);
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
		m_Renderer.SetViewport(width, height);
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
		::ImGui::Text("Frametime: %f", m_DeltaTime);

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
}