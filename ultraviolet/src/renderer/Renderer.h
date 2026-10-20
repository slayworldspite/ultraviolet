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
		void SetClearColor(const glm::vec4& color);
		void Draw(const Mesh& mesh) const;
		void Draw(const Mesh& mesh, const glm::mat4& model) const;
		void Draw(const Model& model) const;
		void Draw(const Entity& entity) const;
		void Draw(const Scene& scene);
		void SetOverlay(std::shared_ptr<Texture> overlay);
		void SetViewport(int width, int height);
		void SetWireframeMode(bool value);
		void SetVSync(bool value) const;
		void Resize(int width, int height);

		void AddToImGui();

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;

		glm::vec4 m_ClearColor;

		bool m_WireframeMode;
		float m_Gamma;
		int m_ShadowMapResolution;
		float m_ShadowCameraSize;
		float m_ShadowCameraDepth;

		int m_ViewportWidth;
		int m_ViewportHeight;

		std::unique_ptr<VertexArray> m_ScreenQuad;

		std::unique_ptr<Framebuffer> m_ScreenFramebuffer;
		std::unique_ptr<Renderbuffer> m_ScreenRenderbuffer;

		std::unique_ptr<Framebuffer> m_PostProcessFramebuffer;

		std::unique_ptr<Framebuffer> m_ShadowMapFramebuffer;
		std::unique_ptr<Shader> m_ShadowMapShader;
		std::unique_ptr<UniformBuffer> m_ShadowMapUniformBuffer;

		glm::mat4 m_LightSpaceMatrix;
		
		std::shared_ptr<Texture> m_OverlayTexture;

		std::unique_ptr<Shader> m_PostProcessingShader;

	private:
		void CreateContext(const NativeWindow& window);
		void InitializeImGui() const;
		void InitializeFramebuffers(const std::pair<int, int>& windowSize);
		
		void DrawShadowMap(const Scene& scene);
		void DrawShadowMap(const Model& model);
		void PresentFrame() const;
	};
}