#include "Mesh.h"

namespace ultraviolet
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<IMaterial> material) :
		Material{ std::move(material) },
		m_VertexBuffer{ vertices },
		m_ElementBuffer{ indices },
		m_VertexArray{ m_VertexBuffer, m_ElementBuffer },
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