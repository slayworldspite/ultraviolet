#pragma once

#include "objects/Texture.h"

namespace ultraviolet
{
	struct RenderTarget
	{
		Texture* Texture;
		size_t Attachment;
	};
}