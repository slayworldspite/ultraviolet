#pragma once

#include <glm/vec4.hpp>

#include "TextureInternalFormat.h"
#include "TextureFormat.h"
#include "TextureFilter.h"
#include "TextureWrap.h"

namespace ultraviolet
{
	struct TextureSettings
	{
		int Width;
		int Height;
		TextureInternalFormat InternalFormat;
		TextureFormat Format;
		TextureFilter MinFiltering;
		TextureFilter MagFiltering;
		TextureWrap Wrap;
		glm::vec4 BorderColor{ 0.0f };
	};
}