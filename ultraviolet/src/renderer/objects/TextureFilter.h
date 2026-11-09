#pragma once

namespace ultraviolet
{
	enum class TextureFilter
	{
		Nearest = 0x2600, // GL_NEAREST
		NearestMipmapNearest = 0x2700, // GL_NEAREST_MIPMAP_NEAREST
		NearestMipmapLinear = 0x2702, // GL_NEAREST_MIPMAP_LINEAR
		Linear = 0x2601, // GL_LINEAR
		LinearMipmapNearest = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
		LinearMipmapLinear = 0x2703 // GL_LINEAR_MIPMAP_LINEAR
	};
}