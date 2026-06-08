#pragma once

#include <glm/glm.hpp>

#include "../core/NativeWindow.h"

namespace ultraviolet
{
	class Renderer
	{
	public:
		Renderer(const NativeWindow& window);

		void BeginFrame();
		void EndFrame();
		void SetClearColor(const glm::vec4& color);

	private:
		::SDL_Window* m_Window;
		void* m_GLContext;
	};
}