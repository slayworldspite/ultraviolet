#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>

#include "GameWindow.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	GameWindow::GameWindow(const NativeWindowSettings& nativeWindowSettings) :
		NativeWindow{ nativeWindowSettings }
	{

	}

	void GameWindow::Run()
	{
		OnStart();

		Logger::Log("game started!", Logger::Level::Info);
		
		uint64_t currentFrameTime{};
		uint64_t lastFrameTime{};

		while (!m_ShouldClose)
		{
			::SDL_Event event;
			while (::SDL_PollEvent(&event))
			{
				ImGui_ImplSDL3_ProcessEvent(&event);

				switch (event.type)
				{
					case ::SDL_EVENT_QUIT:
					case ::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						m_ShouldClose = true;
						OnClose();
						break;

					case ::SDL_EVENT_MOUSE_MOTION:
						if (!ImGui::GetIO().WantCaptureMouse)
							OnMouseMove(event.motion.xrel, event.motion.yrel);
						break;

					case ::SDL_EVENT_MOUSE_BUTTON_DOWN:
						if (!ImGui::GetIO().WantCaptureMouse)
							OnMouseClick();
						break;

					case ::SDL_EVENT_KEY_DOWN:
						if (!ImGui::GetIO().WantCaptureKeyboard)
							OnKeyPress(static_cast<Keycode>(event.key.key));
						break;

					case ::SDL_EVENT_WINDOW_RESIZED:
						OnResize(event.window.data1, event.window.data2);
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

	void GameWindow::SetShouldClose(bool value)
	{
		m_ShouldClose = value;
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

	void GameWindow::OnKeyPress(Keycode key)
	{

	}

	void GameWindow::OnMouseClick()
	{

	}

	void GameWindow::OnMouseMove(float x, float y)
	{

	}

	void GameWindow::OnResize(int width, int height)
	{

	}

	void GameWindow::OnClose()
	{

	}
}