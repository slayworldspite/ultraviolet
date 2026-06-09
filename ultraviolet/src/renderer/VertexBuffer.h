#pragma once

#include <vector>

class VertexBuffer
{
public:
	VertexBuffer(const std::vector<float>& data);

	void Bind() const;
	void Unbind() const;
	uint32_t GetID() const;

	operator uint32_t() const;

private:
	uint32_t m_ID;
};