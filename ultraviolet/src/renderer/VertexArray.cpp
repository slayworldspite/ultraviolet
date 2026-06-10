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