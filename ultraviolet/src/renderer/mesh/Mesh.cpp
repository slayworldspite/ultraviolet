#include "Mesh.h"

namespace ultraviolet
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<IMaterial> material) :
		Material{ std::move(material) },
		m_VertexBuffer{ vertices },
		m_ElementBuffer{ indices },
		m_VertexArray{ m_VertexBuffer, m_ElementBuffer },
		m_IndexCount{ indices.size() }
	{

	}

	void Mesh::Bind() const
	{
		m_VertexArray.Bind();
	}

	size_t Mesh::GetIndexCount() const
	{
		return m_IndexCount;
	}
}