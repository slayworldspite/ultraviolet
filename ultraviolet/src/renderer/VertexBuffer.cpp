#include <glad/glad.h>

#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const std::vector<float>& data) :
	m_ID{}
{
	::glGenBuffers(1, &m_ID);
	
	Bind();
	::glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
	::glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::Unbind() const
{
	::glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint32_t VertexBuffer::GetID() const
{
	return m_ID;
}

VertexBuffer::operator uint32_t() const
{
	return m_ID;
}