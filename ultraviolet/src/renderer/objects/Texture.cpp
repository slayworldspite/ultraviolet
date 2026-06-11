#include <glad/glad.h>

#include "Texture.h"

namespace ultraviolet
{
	Texture::Texture(const std::filesystem::path& path)
	{

	}

	Texture::~Texture()
	{
		Delete();
	}

	void Texture::Bind(int index) const
	{
		::glActiveTexture(GL_TEXTURE0 + index);
		::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Bind() const
	{
		::glActiveTexture(GL_TEXTURE0);
		::glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Unbind(int index) const
	{
		::glActiveTexture(GL_TEXTURE0 + index);
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