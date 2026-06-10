#pragma once

#include <cstdint>

namespace ultraviolet
{
	enum class MouseButtonFlags : uint32_t
	{
		Left = 1 << 0,
		Middle = 1 << 1,
		Right = 1 << 2,
		X1 = 1 << 3,
		X2 = 1 << 4
	};
}