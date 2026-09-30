#pragma once

#include "GLObject.h"
#include "Texture.h"

namespace ultraviolet
{
	class Framebuffer : public GLObject
	{
	public:
		Texture Texture;

	public:
		Framebuffer(int width, int height, int internalFormat, int format, int type, int attachment, bool multisampled = false);

		virtual ~Framebuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		void AddToImGui(const std::string& id);
	};
}