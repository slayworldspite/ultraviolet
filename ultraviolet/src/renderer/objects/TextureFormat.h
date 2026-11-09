#pragma once

#include <glad/glad.h>

namespace ultraviolet
{
	enum class TextureFormat
	{
		Red = 0x1903, // GL_RED
		Rg = 0x8227, // GL_RG
		Rgb = 0x1907, // GL_RGB
		Rgba = 0x1908, // GL_RGBA
		DepthComponent = 0x1902, // GL_DEPTH_COMPONENT
		DepthStencil = 0x84F9 // GL_DEPTH_STENCIL
	};
}