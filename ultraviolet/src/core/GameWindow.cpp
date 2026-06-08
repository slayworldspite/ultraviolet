#include <SDL3/SDL.h>

#include "GameWindow.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	GameWindow::GameWindow(const NativeWindowSettings& nativeWindowSettings, const GameWindowSettings& gameWindowSettings) :
		NativeWindow{ nativeWindowSettings },
		m_UpdateFrequency{ gameWindowSettings.UpdateFrequency }
	{

	}

	void GameWindow::Run()
	{
		Logger::Log("game started!", Logger::Level::Info);
		
		uint64_t currentFrameTime{};
		uint64_t lastFrameTime{};

		while (!m_ShouldClose)
		{
			::SDL_Event event;
			while (::SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case ::SDL_EVENT_QUIT:
						m_ShouldClose = true;
						break;

					case ::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						m_ShouldClose = true;
						break;
				}
			}

			lastFrameTime = currentFrameTime;
			currentFrameTime = ::SDL_GetTicksNS();
			double deltaTime{ static_cast<double>(currentFrameTime - lastFrameTime) / 1'000'000'000.0 };

			OnUpdate(deltaTime);
			OnRender();
		}

		Logger::Log("closing...", Logger::Level::Info);
	}

	void GameWindow::OnStart()
	{

	}

	void GameWindow::OnUpdate(double deltaTime)
	{
		
	}

	void GameWindow::OnRender()
	{

	}
}