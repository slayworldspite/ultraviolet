#pragma once

#include "GLObject.h"
#include "Texture.h"
#include "FramebufferAttachment.h"

namespace ultraviolet
{
	class Framebuffer : public GLObject
	{
	public:
		std::vector<Texture> Buffers;

	public:
		Framebuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment);
		Framebuffer(const TextureSettings& textureSettings, size_t textureCount);
		Framebuffer(const std::vector<TextureSettings>& textureSettings);

		virtual ~Framebuffer() override;

		void AddBuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment);
		void Clear();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		void AddToImGui();
	};
}