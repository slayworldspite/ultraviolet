#pragma once

namespace ultraviolet
{
	enum class TextureInternalFormat
	{
		Rgb = 0x8051, // GL_RGB8
		Rgb32f = 0x8815, // GL_RGB32F
		Rgba = 0x8058, // GL_RGBA8
		Rgba32f = 0x8814, // GL_RGBA32F
		Srgb = 0x8C41, // GL_SRGB8
		Srgba = 0x8C34, // GL_SRGB8_ALPHA
		DepthComponent = 0x8CAC, // GL_DEPTH_COMPONENT32F 
		DepthStencil = 0x8CAD // GL_DEPTH32F_STENCIL8
	};
}