#pragma once

#include "../core/NativeWindow.h"

namespace
{
	struct SDL_GLContext;
}

namespace ultraviolet
{
	namespace RenderContext
	{
		void Initialize(const NativeWindow& window);
		::SDL_GLContext GetContext();
		bool IsInitialized();
	};
}