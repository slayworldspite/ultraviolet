#pragma once

#include "../core/NativeWindow.h"

namespace ultraviolet
{
	class Renderer
	{
	public:
		Renderer(const NativeWindow& window);

	private:
		void* m_GLContext;
	};
}