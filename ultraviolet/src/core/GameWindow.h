#pragma once

#include "NativeWindow.h"
#include "GameWindowSettings.h"

namespace ultraviolet
{
	class GameWindow : public NativeWindow
	{
	public:
		GameWindow(const NativeWindowSettings& nativeWindowSettings, const GameWindowSettings& gameWindowSettings);

		void Run();

	protected:
		virtual void OnStart();
		virtual void OnUpdate(double deltaTime);
		virtual void OnRender();

	private:
		float m_UpdateFrequency;
		bool m_ShouldClose{ false };
	};
}
