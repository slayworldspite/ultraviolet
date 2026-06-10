#include "Mesh.h"

namespace ultraviolet
{
	Mesh::Mesh(const std::vector<Vertex>& vertices) :
		m_VertexBuffer{ vertices },
		m_VertexArray{ m_VertexBuffer },
		m_VertexCount{ vertices.size() }
	{

	}

	void Mesh::Bind() const
	{
		m_VertexArray.Bind();
	}

	size_t Mesh::GetVertexCount() const
	{
		return m_VertexCount;
	}
}