#include <SDL3/SDL.h>

#include "NativeWindow.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	NativeWindow::NativeWindow(const NativeWindowSettings& settings)
	{
		Logger::Log("creating window...", Logger::Level::Info);

		if (!::SDL_Init(SDL_INIT_VIDEO))
		{
			Logger::Log("failed to initialize sdl!", Logger::Level::Critical);
			exit(-1);
		}

		::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_MINOR_VERSION, 6);
		::SDL_GL_SetAttribute(::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		::SDL_GL_SetAttribute(::SDL_GL_MULTISAMPLEBUFFERS, 1);
		::SDL_GL_SetAttribute(::SDL_GL_MULTISAMPLESAMPLES, 4);

		m_Window = ::SDL_CreateWindow(
			settings.Title.c_str(),
			settings.Width,
			settings.Height,
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE
		);

		if (!m_Window)
		{
			Logger::Log("failed to create window!", Logger::Level::Critical);
			exit(-1);
		}

		Logger::Log("window created!", Logger::Level::Info);
	}

	NativeWindow::~NativeWindow()
	{
		Logger::Log("destroying window...", Logger::Level::Info);

		::SDL_DestroyWindow(m_Window);

		Logger::Log("window destroyed!", Logger::Level::Info);

		::SDL_Quit(); // shh :3
	}

	::SDL_Window* NativeWindow::GetNativeHandle() const
	{
		return m_Window;
	}

	std::pair<int, int> NativeWindow::GetWindowSize() const
	{
		int width{};
		int height{};

		::SDL_GetWindowSizeInPixels(m_Window, &width, &height);

		return { width, height };
	}

	void NativeWindow::SetRelativeMouseMode(bool value) const
	{
		::SDL_SetWindowRelativeMouseMode(m_Window, value);
	}

	bool NativeWindow::GetRelativeMouseMode() const
	{
		return ::SDL_GetWindowRelativeMouseMode(m_Window);
	}

	void NativeWindow::SetFullscreen(bool value) const
	{
		::SDL_SetWindowFullscreen(m_Window, value);
	}
}