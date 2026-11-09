#pragma once

namespace ultraviolet
{
	enum class FramebufferAttachment
	{
		Color = 0x8CE0, // GL_COLOR_ATTACHMENT0 - Cast to int and add index
		Depth = 0x8D00, // GL_DEPTH_ATTACHMENT
		Stencil = 0x8D20, // GL_STENCIL_ATTACHMENT
		DepthStencil = 0x821A // GL_DEPTH_STENCIL_ATTACHMENT
	};
}