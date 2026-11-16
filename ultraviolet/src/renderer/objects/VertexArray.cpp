#include <glad/glad.h>

#include "VertexArray.h"

namespace ultraviolet
{
	VertexArray::VertexArray(const VertexBuffer& vertexBuffer) :
		GLObject{}
	{
		::glGenVertexArrays(1, &m_ID);

		Bind();
		vertexBuffer.Bind();

		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

		::glEnableVertexAttribArray(1);
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		::glEnableVertexAttribArray(2);
		::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoordinates));

		::glEnableVertexAttribArray(3);
		::glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		Unbind();
	}

	VertexArray::VertexArray(const VertexBuffer& vertexBuffer, const ElementBuffer& elementBuffer) :
		GLObject{}
	{
		::glGenVertexArrays(1, &m_ID);

		Bind();
		vertexBuffer.Bind();
		elementBuffer.Bind();

		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

		::glEnableVertexAttribArray(1);
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		::glEnableVertexAttribArray(2);
		::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoordinates));

		::glEnableVertexAttribArray(3);
		::glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		Unbind();
	}

	VertexArray::VertexArray(VertexArray&& other) noexcept
	{
		m_ID = other.m_ID;
		other.m_ID = 0;
	}

	VertexArray::~VertexArray()
	{
		Delete();
	}

	VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
	{
		if (this != &other)
		{
			::glDeleteVertexArrays(1, &other.m_ID);

			m_ID = other.m_ID;
			other.m_ID = 0;
		}

		return *this;
	}

	void VertexArray::Bind() const
	{
		::glBindVertexArray(m_ID);
	}

	void VertexArray::Unbind() const
	{
		::glBindVertexArray(0);
	}

	void VertexArray::Delete()
	{
		::glDeleteVertexArrays(1, &m_ID);
	}
}