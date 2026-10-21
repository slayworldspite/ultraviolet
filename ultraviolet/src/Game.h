#pragma once

#include <box3d/box3d.h>

#include "core/GameWindow.h"
#include "renderer/Renderer.h"
#include "renderer/camera/ProjectionCamera.h"
#include "renderer/materials/LightMaterial.h"
#include "game/characters/Player.h"

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
		double m_DeltaTime;
		Scene m_Scene;

		bool m_Fullscreen;
		bool m_ShowMenu;
		bool m_FreeCam;
		glm::vec4 m_ClearColor;
		std::unique_ptr<Player> m_Player;

	private:
		void ProcessInput(double deltaTime);
		void DebugMenu();
	};
}