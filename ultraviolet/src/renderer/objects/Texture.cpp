#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

#include "Texture.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Texture::Texture(const std::filesystem::path& path, TextureType type) :
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
			GLenum imageFormat{};

			switch (channelCount)
			{
				case 1:
					imageFormat = GL_RED;
					break;

				case 3:
					imageFormat = GL_RGB;
					break;

				case 4:
					imageFormat = GL_RGBA;
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
		::glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + static_cast<int>(Type)));
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