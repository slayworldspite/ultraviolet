#pragma once

#include <utility>

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

		::SDL_Window* GetNativeHandle() const;
		std::pair<int, int> GetWindowSize() const;
		void SetRelativeMouseMode(bool value) const;
		bool GetRelativeMouseMode() const;
		void SetFullscreen(bool value) const;

	private:
		::SDL_Window* m_Window;
	};
}