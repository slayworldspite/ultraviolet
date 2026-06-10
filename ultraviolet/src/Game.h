#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/mesh/Mesh.h"
#include "renderer/camera/ProjectionCamera.h"

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

	private:
		Renderer m_Renderer;
		Shader m_Shader;
		Mesh m_Mesh;
		ProjectionCamera m_Camera;

	private:
		void ProcessInput(double deltaTime);
	};
}