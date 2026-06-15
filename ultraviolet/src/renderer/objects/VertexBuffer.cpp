#include <glad/glad.h>

#include "VertexBuffer.h"

namespace ultraviolet
{
	VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices) :
		GLObject{}
	{
		::glGenBuffers(1, &m_ID);

		Bind();

		::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		Unbind();
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
	{
		m_ID = other.m_ID;
		other.m_ID = 0;
	}

	VertexBuffer::~VertexBuffer()
	{
		Delete();
	}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_ID);

			m_ID = other.m_ID;
			other.m_ID = 0;
		}

		return *this;
	}

	void VertexBuffer::Bind() const
	{
		::glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void VertexBuffer::Unbind() const
	{
		::glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::Delete()
	{
		::glDeleteBuffers(1, &m_ID);
	}

}