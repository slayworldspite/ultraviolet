#pragma once

#include <utility>

#include "Scancode.h"
#include "MouseButtonFlags.h"

namespace ultraviolet
{
	namespace Input
	{
		bool IsKeyHeld(Scancode key);
		std::pair<float, float> GetMousePosition();
		MouseButtonFlags GetMouseState();
	};
}