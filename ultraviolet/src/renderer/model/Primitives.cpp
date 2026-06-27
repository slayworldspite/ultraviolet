#include "Primitives.h"

namespace ultraviolet
{
	Model Primitives::GetPlane(std::string_view id, std::shared_ptr<IMaterial> material)
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(4);

		Vertex vertex{};
		vertex.Position = { -1.0f, 0.0f, 1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, 1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		glm::vec3 edge1{ vertices[1].Position - vertices[0].Position };
		glm::vec3 edge2{ vertices[2].Position - vertices[0].Position };
		glm::vec2 deltaUV1{ vertices[1].TextureCoordinates - vertices[0].TextureCoordinates };
		glm::vec2 deltaUV2{ vertices[2].TextureCoordinates - vertices[0].TextureCoordinates };

		float fraction{ 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y) };

		glm::vec3 tangent{};
		tangent.x = fraction * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = fraction * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = fraction * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		for (Vertex& vertex : vertices)
			vertex.Tangent = tangent;

		Mesh mesh{ id, vertices, {0, 2, 1, 1, 2, 3}, std::move(material) };

		std::vector<Mesh> meshes{};
		meshes.push_back(std::move(mesh));

		Model model{ id, meshes };

		return model;
	}
}