#pragma once

#include "Framebuffer.h"

namespace ultraviolet
{
	class Renderbuffer : public GLObject
	{
	public:
		Renderbuffer(const Framebuffer& framebuffer, bool multisampled = false);

		virtual ~Renderbuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;
	};
}