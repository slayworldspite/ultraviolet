#include <cstdint>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Texture::Texture(const std::filesystem::path& path, TextureType type) :
		GLObject{},
		Type{ type }
	{
			Logger::Log("loading " + path.string(), Logger::Level::Info);

		int width{};
		int height{};
		int channelCount{};

		::stbi_set_flip_vertically_on_load(1);
		uint8_t* data{ ::stbi_load(path.string().c_str(), &width, &height, &channelCount, 0) };
		
		m_Size.first = width;
		m_Size.second = height;

		::glGenTextures(1, &m_ID);
		Bind();

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (data)
		{
			GLenum internalFormat{};
			GLenum imageFormat{};

			switch (channelCount)
			{
				case 1:
					internalFormat = GL_RED;
					imageFormat = GL_RED;
					break;

				case 3:
					if (type == TextureType::Diffuse)
					{
						internalFormat = GL_SRGB;
						imageFormat = GL_RGB;
					}
					else
					{
						internalFormat = GL_RGB;
						imageFormat = GL_RGB;
					}
					break;

				case 4:
					if (type == TextureType::Diffuse)
					{
						internalFormat = GL_SRGB_ALPHA;
						imageFormat = GL_RGBA;
					}
					else
					{
						internalFormat = GL_RGBA;
						imageFormat = GL_RGBA;
					}
					break;
			}

			::glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
			::glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			Logger::Log("failed to load texture!", Logger::Level::Error);
		}

		::stbi_image_free(data);

		Unbind();
	}

	Texture::Texture(const TextureSettings& settings) :
		GLObject{},
		Type{ TextureType::Framebuffer }
	{
		m_Size.first = settings.Width;
		m_Size.second = settings.Height;

		::glGenTextures(1, &m_ID);
		Bind();

		::glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(settings.InternalFormat), settings.Width, settings.Height, 0, static_cast<GLenum>(settings.Format), GL_FLOAT, nullptr);

		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(settings.MinFiltering));
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(settings.MagFiltering));
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(settings.Wrap));
		::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(settings.Wrap));
		
		if (settings.Wrap == TextureWrap::ClampToBorder)
			::glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(settings.BorderColor));
	}

	Texture::Texture(Texture&& other) noexcept
	{
		m_ID = other.m_ID;
		Type = other.Type;
		m_Size = other.m_Size;

		other.m_ID = 0;
	}

	Texture::~Texture()
	{
		Delete();
	}

	std::pair<int, int> Texture::GetSize() const
	{
		return m_Size;
	}

	void Texture::Bind(size_t index) const
	{
		::glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + static_cast<int>(index)));

		::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Bind() const
	{
		::glActiveTexture(GL_TEXTURE0);

		::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Unbind(size_t index) const
	{
		::glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));

		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Unbind() const
	{
		::glActiveTexture(GL_TEXTURE0);

		::glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Delete()
	{
		::glDeleteTextures(1, &m_ID);
	}
}