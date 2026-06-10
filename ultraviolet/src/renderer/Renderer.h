#pragma once

#include <glm/glm.hpp>

#include "../core/NativeWindow.h"
#include "Mesh.h"
#include "Shader.h"

namespace ultraviolet
{
	class Renderer
	{
	public:
		Renderer(const NativeWindow& window);

		void BeginFrame() const;
		void EndFrame() const;
		void SetClearColor(const glm::vec4& color);
		void Draw(const Mesh& mesh, const Shader& shader) const;

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;
	};
}