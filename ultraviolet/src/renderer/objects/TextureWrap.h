#pragma once

namespace ultraviolet
{
	enum class TextureWrap
	{
		Repeat = 0x2901, // GL_REPEAT
		MirroredRepeat = 0x8370, // GL_MIRRORED_REPEAT
		ClampToEdge = 0x812F, // GL_CLAMP_TO_EDGE
		ClampToBorder = 0x812D // GL_CLAMP_TO_BORDER
	};
}