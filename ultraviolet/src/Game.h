#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"
#include "renderer/camera/ProjectionCamera.h"
#include "renderer/materials/LightMaterial.h"
#include "game/Player.h"

namespace ultraviolet
{
	class Game : public GameWindow
	{
	public:
		Game();

	protected:
		void OnStart() override;
		void OnUpdate(double deltaTime) override;
		void OnRender() override;
		void OnKeyPress(Keycode key) override;
		void OnMouseMove(float x, float y) override;
		void OnResize(int width, int height) override;

	private:
		Renderer m_Renderer;
		ProjectionCamera m_Camera;
		double m_DeltaTime;
		Scene m_Scene;

		glm::vec4 m_ClearColor;

		Player m_Player;
		std::shared_ptr<LightMaterial> m_PointLightMaterial;

		std::unique_ptr<Mesh> m_Overlay;

	private:
		void ProcessInput(double deltaTime);
		void LoadScene();
		void DebugMenu();
	};
}