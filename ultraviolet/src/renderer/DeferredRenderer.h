#pragma once

#include <glm/glm.hpp>

#include "IRenderer.h"
#include "../core/NativeWindow.h"
#include "scene/Scene.h"
#include "camera/ProjectionCamera.h"
#include "objects/Framebuffer.h"
#include "RenderQueue.h"
#include "passes/DeferredPass.h"

namespace ultraviolet
{
	class DeferredRenderer : public IRenderer
	{
	public:
		DeferredRenderer(const NativeWindow& window);
		~DeferredRenderer();

		virtual void BeginFrame() const override;
		virtual void EndFrame() const override;

		void Submit(const Mesh& mesh, const glm::mat4& transform = glm::zero<glm::mat4>());
		void Submit(const Model& model);
		void Submit(const Entity& entity);
		void Submit(const Scene& scene);
		virtual void Execute() override;

		void SetViewport(int width, int height);
		void SetWireframeMode(bool value);
		void SetVSync(bool value) const;
		void Resize(int width, int height);

		void AddToImGui();

	private:
		::SDL_Window* m_Window;
		std::unique_ptr<Framebuffer> m_GBuffer;
		std::unique_ptr<Mesh> m_ScreenQuad;
		std::unique_ptr<DeferredPass> m_DeferredPass;
		std::unique_ptr<Shader> m_PostProcessingShader;
		RenderQueue m_RenderQueue;
		bool m_WireframeMode;
		float m_Gamma;
		bool m_EnableFXAA;
		int m_ViewportWidth;
		int m_ViewportHeight;

	private:
		void CreateGBuffer();
		void InitializeRenderPasses();
		void InitializePostProcessingShader();
		void InitializeImGui() const;
		void Draw(const DrawItem& drawItem) const;
		void PresentFrame() const;
	};
}