#include <SDL3/SDL.h>

#include "NativeWindow.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	NativeWindow::NativeWindow(const NativeWindowSettings& settings)
	{
		Logger::Log("creating window...", Logger::Level::Info);

		::SDL_Init(SDL_INIT_VIDEO);

		m_Window = ::SDL_CreateWindow(
			settings.Title.c_str(),
			settings.Width,
			settings.Height,
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE
		);

		Logger::Log("window created!", Logger::Level::Info);
	}

	NativeWindow::~NativeWindow()
	{
		Logger::Log("destroying window...", Logger::Level::Info);

		::SDL_DestroyWindow(m_Window);

		Logger::Log("window destroyed!", Logger::Level::Info);

		::SDL_Quit(); // shh :3
	}
}