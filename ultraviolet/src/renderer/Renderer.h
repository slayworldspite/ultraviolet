#pragma once

#include <glm/glm.hpp>

#include "../core/NativeWindow.h"
#include "model/Model.h"
#include "Shader.h"
#include "objects/Framebuffer.h"

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
		void Draw(const Mesh& mesh, const Shader& shader) const;
		void Draw(const Model& model, const Shader& shader) const;
		void SetViewport(int width, int height) const;
		void ToggleWireframeMode() const;
		void SetVSync(bool value) const;
		void SetLightDirection(float yaw, float pitch);

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;
		std::unique_ptr<Framebuffer> m_ShadowBuffer;
		std::unique_ptr<Shader> m_ShadowShader;
		glm::vec3 m_LightDirection;
		glm::mat4 m_LightProjection;
	};
}