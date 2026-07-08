#pragma once

#include <glm/glm.hpp>

#include "../core/NativeWindow.h"
#include "scene/Scene.h"
#include "camera/ProjectionCamera.h"
#include "objects/Renderbuffer.h"

namespace ultraviolet
{
	class Renderer
	{
	public:
		Renderer(const NativeWindow& window);
		~Renderer();

		void BeginFrame() const;
		void EndFrame() const;
		void SetClearColor(const glm::vec4& color) const;
		void Draw(const Mesh& mesh) const;
		void Draw(const Mesh& mesh, const glm::mat4& model) const;
		void Draw(const Model& model) const;
		void Draw(const Scene& scene);
		void SetViewport(int width, int height);
		void SetWireframeMode(bool value);
		void SetVSync(bool value) const;

		void AddToImGui();

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;

		bool m_WireframeMode;
		bool m_EnableBloom;

		int m_ViewportWidth;
		int m_ViewportHeight;
		std::unique_ptr<VertexArray> m_ScreenQuad;

		std::unique_ptr<Framebuffer> m_ScreenFramebuffer;
		std::unique_ptr<Renderbuffer> m_ScreenRenderbuffer;

		std::unique_ptr<Framebuffer> m_PostProcessFramebuffer;

		std::unique_ptr<Framebuffer> m_DepthMapFrameBuffer;
		std::unique_ptr<Shader> m_DepthMapShader;
		std::unique_ptr<UniformBuffer> m_DepthMapUniformBuffer;

		glm::mat4 m_LightSpaceMatrix;

		std::unique_ptr<Framebuffer> m_BrightExtractFramebuffer;
		std::unique_ptr<Shader> m_BrightExtractShader;

		std::unique_ptr<Shader> m_BlurShader;
		std::unique_ptr<Framebuffer> m_HorizontalBlurFramebuffer;
		std::unique_ptr<Framebuffer> m_VerticalBlurFramebuffer;

		std::unique_ptr<Shader> m_PostProcessingShader;
	};
}