#pragma once

#include <utility>

#include "GLObject.h"

namespace ultraviolet
{
	// WARNING: most likely a bad implementation
	class Framebuffer : public GLObject
	{
	public:
		Framebuffer(int width, int height, uint32_t format, uint32_t attachment); // GLenum
		Framebuffer(Framebuffer&& other) noexcept;
		virtual ~Framebuffer() override;
		Framebuffer& operator=(Framebuffer&& other) noexcept;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
		std::pair<int, int> GetSize() const;
		uint32_t GetTexture() const;

	private:
		int m_Width;
		int m_Height;
		uint32_t m_Texture;
	};
}