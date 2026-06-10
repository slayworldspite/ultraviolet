#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>

#include "Input.h"

namespace ultraviolet
{
	bool Input::IsKeyHeld(Scancode key)
	{
		return ::SDL_GetKeyboardState(nullptr)[static_cast<int>(key)];
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		float x{};
		float y{};

		::SDL_GetMouseState(&x, &y);

		return { x, y };
	}

	MouseButtonFlags Input::GetMouseState()
	{
		return static_cast<MouseButtonFlags>(::SDL_GetMouseState(nullptr, nullptr));
	}
}