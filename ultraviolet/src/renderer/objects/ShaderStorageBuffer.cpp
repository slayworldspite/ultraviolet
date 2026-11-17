#include <glad/glad.h>

#include "ShaderStorageBuffer.h"

namespace ultraviolet
{
	ShaderStorageBuffer::ShaderStorageBuffer(const void* data, size_t size, size_t bindingIndex) :
		GLObject{}
	{
		::glGenBuffers(1, &m_ID);
		
		Bind();

		::glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
		::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(bindingIndex), m_ID);

		Unbind();
	}

	ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) :
		GLObject{}
	{
		m_ID = other.m_ID;
		other.m_ID = 0;
	}

	ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other)
	{
		if (this != &other)
		{
			m_ID = other.m_ID;
			other.m_ID = 0;
		}

		return *this;
	}

	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		Delete();
	}

	void ShaderStorageBuffer::SetData(const void* data, size_t size) const
	{
		SetData(data, size, 0);
	}

	void ShaderStorageBuffer::SetData(const void* data, size_t size, size_t offset) const
	{
		Bind();
		::glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
		Unbind();
	}

	void ShaderStorageBuffer::Bind() const
	{
		::glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID);
	}

	void ShaderStorageBuffer::Unbind() const
	{
		::glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void ShaderStorageBuffer::Delete()
	{
		::glDeleteBuffers(1, &m_ID);
	}
}