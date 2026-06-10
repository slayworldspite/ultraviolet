#pragma once

#include <cstdint>

namespace ultraviolet
{
	class GLObject
	{
	public:
		GLObject();
		virtual ~GLObject() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Delete() = 0;
		uint32_t GetID() const;

		operator uint32_t() const;

	protected:
		uint32_t m_ID;
	};
}