#pragma once

#include <glm/glm.hpp>

#include "../core/NativeWindow.h"
#include "scene/Scene.h"
#include "camera/ProjectionCamera.h"
#include "objects/Renderbuffer.h"
#include "RenderQueue.h"

namespace ultraviolet
{
	class Renderer
	{
	public:
		Renderer(const NativeWindow& window);
		~Renderer();

		void BeginFrame() const;
		void EndFrame();

		void Submit(const Mesh& mesh, const glm::mat4& transform = glm::zero<glm::mat4>());
		void Submit(const Model& model);
		void Submit(const Entity& entity);
		void Submit(const Scene& scene);

		void SetClearColor(const glm::vec4& color);
		void SetViewport(int width, int height);
		void SetWireframeMode(bool value);
		void SetVSync(bool value) const;
		void Resize(int width, int height);

		void AddToImGui();

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;

		RenderQueue m_RenderQueue;

		bool m_WireframeMode;
		float m_Gamma;

		int m_ViewportWidth;
		int m_ViewportHeight;

	private:
		void CreateContext(const NativeWindow& window);
		void InitializeImGui() const;
		void Draw(const DrawItem& drawItem) const;
	};
}