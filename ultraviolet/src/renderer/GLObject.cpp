#include <glad/glad.h>

#include "GLObject.h"

GLObject::GLObject() :
	m_ID{}
{

}

uint32_t GLObject::GetID() const
{
	return m_ID;
}

GLObject::operator uint32_t() const
{
	return m_ID;
}