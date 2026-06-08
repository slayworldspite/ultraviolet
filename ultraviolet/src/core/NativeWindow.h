#pragma once

#include "NativeWindowSettings.h"

namespace
{
	struct SDL_Window;
}

namespace ultraviolet
{
	class NativeWindow
	{
	public:
		NativeWindow(const NativeWindowSettings& settings);
		~NativeWindow();

	private:
		::SDL_Window* m_Window;
	};
}