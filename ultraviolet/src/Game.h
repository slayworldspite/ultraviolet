#pragma once

#include "core/GameWindow.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"

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

	private:
		Renderer m_Renderer;
		Shader m_Shader;
		Mesh m_Mesh;
	};
}