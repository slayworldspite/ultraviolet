#include <glad/glad.h>

#include "UniformBuffer.h"

namespace ultraviolet
{
	UniformBuffer::UniformBuffer(const void* data, size_t size, size_t bindingPoint) :
		GLObject{}
	{
		::glGenBuffers(1, &m_ID);

		Bind();

		::glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		::glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(bindingPoint), m_ID);

		Unbind();
	}

	UniformBuffer::~UniformBuffer()
	{
		Delete();
	}

	void UniformBuffer::Bind() const
	{
		::glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	}

	void UniformBuffer::Unbind() const
	{
		::glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBuffer::Delete()
	{
		::glDeleteBuffers(1, &m_ID);
	}

	void UniformBuffer::SetData(const void* data, size_t size) const
	{
		Bind();
		::glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
	}

	void UniformBuffer::SetData(const void* data, size_t size, size_t offset) const
	{
		Bind();
		::glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}
}