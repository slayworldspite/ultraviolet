#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/model/Model.h"
#include "renderer/camera/ProjectionCamera.h"
#include "renderer/lights/AmbientLight.h"

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
		Shader m_Shader;

		ProjectionCamera m_Camera;

		double m_DeltaTime;

		AmbientLight m_AmbientLight;

		std::vector<Model> m_Models;

	private:
		void ProcessInput(double deltaTime);
		void LoadAssets();
		void DebugMenu();
	};
}