#include <glad/glad.h>

#include "VertexArray.h"

VertexArray::VertexArray(const VertexBuffer& vertexBuffer, const std::vector<size_t>& sizes) :
	GLObject{}
{
	::glGenVertexArrays(1, &m_ID);

	vertexBuffer.Bind();

	size_t sum{};

	for (int size : sizes)
		sum += size;

	size_t stride{ sum * sizeof(float) };

	for (size_t i{}; i < sizes.size(); i++)
	{
		int lastOffset{};

		::glEnableVertexAttribArray(i);
		::glVertexAttribPointer(i, sizes[i], GL_FLOAT, false, stride, &lastOffset);

		lastOffset += sizes[i] * sizeof(float);
	}
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