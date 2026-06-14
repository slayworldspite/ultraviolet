#include <glad/glad.h>

#include "ElementBuffer.h"

namespace ultraviolet
{
	ElementBuffer::ElementBuffer(const std::vector<uint32_t>& indices) :
		GLObject{}
	{
		::glGenBuffers(1, &m_ID);
		Bind();

		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}	

	ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept
	{
		m_ID = other.m_ID;
		other.m_ID = 0;
	}

	ElementBuffer::~ElementBuffer()
	{
		Delete();
	}

	ElementBuffer& ElementBuffer::operator=(ElementBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_ID);

			m_ID = other.m_ID;
			other.m_ID = 0;
		}

		return *this;
	}

	void ElementBuffer::Bind() const
	{
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void ElementBuffer::Unbind() const
	{
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void ElementBuffer::Delete()
	{
		::glDeleteBuffers(1, &m_ID);
	}
}