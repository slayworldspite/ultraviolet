#include "Primitives.h"
#include "../materials/ColorMaterial.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Model Primitives::GetPlane(std::string_view id, std::shared_ptr<Material> material)
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(4);

		Vertex vertex{};
		vertex.Position = { -1.0f, 0.0f, 1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, 1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		Mesh mesh{ id, vertices, {0, 2, 1, 1, 2, 3} };

		if (material)
			mesh.Material = material;
		else
			mesh.Material = std::make_shared<ColorMaterial>(glm::vec3{ 1.0f }, 32);

		std::vector<Mesh> meshes{};
		meshes.push_back(std::move(mesh));

		Model model{ id, meshes };

		return model;
	}
}