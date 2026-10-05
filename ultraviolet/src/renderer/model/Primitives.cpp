#include <numbers>

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
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, 1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 1.0f, 0.0f, -1.0f };
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertices.push_back(vertex);

		Mesh mesh{ id, vertices, { 0, 2, 1, 1, 2, 3 } };

		if (material)
			mesh.Material = material;
		else
			mesh.Material = std::make_shared<ColorMaterial>(glm::vec3{ 1.0f }, 32);

		std::vector<Mesh> meshes{};
		meshes.push_back(std::move(mesh));

		Model model{ id, meshes };

		return model;
	}

	Model Primitives::GetCube(std::string_view id, std::shared_ptr<Material> material)
	{
		std::vector<Vertex> vertices{};
		vertices.reserve(24);

		Vertex vertex{};

		// front
		vertex.Normal = { 0.0f, 0.0f, 1.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };

		vertex.Position = { -0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		// back
		vertex.Normal = { 0.0f, 0.0f, -1.0f };
		vertex.Tangent = { -1.0f, 0.0f, 0.0f };

		vertex.Position = { 0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		// left
		vertex.Normal = { -1.0f, 0.0f, 0.0f };
		vertex.Tangent = { 0.0f, 0.0f, 1.0f };

		vertex.Position = { -0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		// right
		vertex.Normal = { 1.0f, 0.0f, 0.0f };
		vertex.Tangent = { 0.0f, 0.0f, -1.0f };

		vertex.Position = { 0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		// top
		vertex.Normal = { 0.0f, 1.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };

		vertex.Position = { -0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, 0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		// bottom
		vertex.Normal = { 0.0f, -1.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };

		vertex.Position = { -0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 0.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, -0.5f, -0.5f };
		vertex.TextureCoordinates = { 1.0f, 0.0f };
		vertices.push_back(vertex);

		vertex.Position = { -0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 0.0f, 1.0f };
		vertices.push_back(vertex);

		vertex.Position = { 0.5f, -0.5f, 0.5f };
		vertex.TextureCoordinates = { 1.0f, 1.0f };
		vertices.push_back(vertex);


		std::vector<unsigned int> indices{
			0, 1, 2, 1, 3, 2,
			4, 5, 6, 5, 7, 6,
			8, 9, 10, 9, 11, 10,
			12, 13, 14, 13, 15, 14,
			16, 17, 18, 17, 19, 18,
			20, 21, 22, 21, 23, 22
		};

		Mesh mesh{ id, vertices, indices };

		if (material)
			mesh.Material = material;
		else
			mesh.Material = std::make_shared<ColorMaterial>(glm::vec3{ 1.0f }, 32);

		std::vector<Mesh> meshes{};
		meshes.push_back(std::move(mesh));

		return Model{ id, meshes };
	}

	// https://stackoverflow.com/a/4082020
	Model Primitives::GetSphere(std::string_view id, size_t horizontal, size_t vertical, std::shared_ptr<Material> material)
	{
		const float pi{ std::numbers::pi_v<float> };

		std::vector<Vertex> vertices{};

		for (size_t horizontalIndex{}; horizontalIndex <= horizontal; horizontalIndex++)
		{
			for (size_t verticalIndex{}; verticalIndex <= vertical; verticalIndex++)
			{
				Vertex vertex{};

				float theta{ pi * horizontalIndex / horizontal };
				float phi{ 2.0f * pi * verticalIndex / vertical };

				vertex.Position.x = sinf(theta) * cosf(phi);
				vertex.Position.y = cosf(theta);
				vertex.Position.z = sinf(theta) * sinf(phi);

				vertex.Normal = vertex.Position;	

				vertex.TextureCoordinates.x = static_cast<float>(verticalIndex) / vertical;
				vertex.TextureCoordinates.y = static_cast<float>(horizontalIndex) / horizontal;

				vertex.Tangent = glm::normalize(
					glm::vec3{ 
						-sinf(phi), 
						cosf(phi), 
						0.0f 
					}
				);

				vertices.push_back(vertex);
			}
		}

		std::vector<uint32_t> indices{};

		const uint32_t stride{ static_cast<uint32_t>(vertical) + 1 };

		for (uint32_t y{}; y < horizontal; y++)
		{
			for (uint32_t x{}; x < vertical; x++)
			{
				uint32_t i0{ y * stride + x };
				uint32_t i1{ i0 + 1 };
				uint32_t i2{ (y + 1) * stride + x };
				uint32_t i3{ i2 + 1 };

				indices.push_back(i0);
				indices.push_back(i2);
				indices.push_back(i1);

				indices.push_back(i1);
				indices.push_back(i2);
				indices.push_back(i3);
			}
		}

		Mesh mesh{ id, vertices, indices};

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