#include <glad/glad.h>

#include "GLObject.h"

namespace ultraviolet
{
	GLObject::GLObject() :
		m_ID{}
	{

	}

	uint32_t GLObject::GetID() const
	{
		return m_ID;
	}
}