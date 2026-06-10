#pragma once

#include <memory>

#include "objects/VertexArray.h"
#include "Vertex.h"

namespace ultraviolet
{
	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices);

		void Bind() const;
		size_t GetVertexCount() const;

	private:
		VertexBuffer m_VertexBuffer;
		VertexArray m_VertexArray;
		size_t m_VertexCount;
	};
}