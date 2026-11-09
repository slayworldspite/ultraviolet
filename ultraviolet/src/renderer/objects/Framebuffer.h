#pragma once

#include "GLObject.h"
#include "Texture.h"
#include "FramebufferAttachment.h"

namespace ultraviolet
{
	class Framebuffer : public GLObject
	{
	public:
		std::vector<Texture> Textures;

	public:
		Framebuffer(const TextureSettings& textureSettings, FramebufferAttachment attachment);
		Framebuffer(const TextureSettings& textureSettings, size_t textureCount);
		Framebuffer(const std::vector<TextureSettings>& textureSettings);

		virtual ~Framebuffer() override;

		void AddTexture(const TextureSettings& textureSettings, FramebufferAttachment attachment);

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Delete() override;

		void AddToImGui();
	};
}