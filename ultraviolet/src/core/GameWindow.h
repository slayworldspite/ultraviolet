#pragma once

#include "NativeWindow.h"
#include "GameWindowSettings.h"
#include "../input/Keycode.h"

namespace ultraviolet
{
	class GameWindow : public NativeWindow
	{
	public:
		GameWindow(const NativeWindowSettings& nativeWindowSettings, const GameWindowSettings& gameWindowSettings);

		void Run();
		void SetShouldClose(bool value);

	protected:
		virtual void OnStart();
		virtual void OnUpdate(double deltaTime);
		virtual void OnRender();
		virtual void OnKeyPress(Keycode key);
		virtual void OnMouseClick();
		virtual void OnMouseMove(float x, float y);
		virtual void OnClose();

	private:
		float m_UpdateFrequency;
		bool m_ShouldClose{ false };
	};
}
