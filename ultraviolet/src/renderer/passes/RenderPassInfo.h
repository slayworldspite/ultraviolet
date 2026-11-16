#pragma once

#include "RenderTarget.h"

namespace ultraviolet
{
	struct RenderPassInfo
	{
		std::vector<RenderTarget> ColorAttachments;
		RenderTarget DepthAttachment;
	};
}