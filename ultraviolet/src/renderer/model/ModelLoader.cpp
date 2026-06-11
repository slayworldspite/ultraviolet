#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include "ModelLoader.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Mesh ModelLoader::Load(const std::filesystem::path& path)
	{
		Logger::Log("loading " + path.string(), Logger::Level::Info);

		tinyobj::ObjReaderConfig readerConfig{};
		readerConfig.mtl_search_path = "";

		tinyobj::ObjReader reader{};

		if (!reader.ParseFromFile(path.string(), readerConfig))
		{
			Logger::Log("tinyobjloader: " + reader.Error(), Logger::Level::Error);
		}

		if (!reader.Warning().empty())
		{
			Logger::Log("tinyobjloader: " + reader.Warning(), Logger::Level::Warning);
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		std::vector<Vertex> vertices{};	

		for (auto& shape : shapes)
		{
			size_t indexOffset{};

			for (auto& face : shape.mesh.num_face_vertices)
			{
				for (size_t vertexIndex{}; vertexIndex < face; vertexIndex++)
				{
					tinyobj::index_t index{ shape.mesh.indices[indexOffset + vertexIndex] };

					Vertex vertex{};

					vertex.Position.x = attrib.vertices[3 * index.vertex_index + 0];
					vertex.Position.y = attrib.vertices[3 * index.vertex_index + 1];
					vertex.Position.z = attrib.vertices[3 * index.vertex_index + 2];

					if (index.normal_index >= 0)
					{
						vertex.Normals.x = attrib.normals[3 * index.normal_index + 0];
						vertex.Normals.y = attrib.normals[3 * index.normal_index + 1];
						vertex.Normals.z = attrib.normals[3 * index.normal_index + 2];
					}

					if (index.texcoord_index >= 0)
					{
						vertex.TextureCoordinates.x = attrib.texcoords[2 * index.texcoord_index + 0];
						vertex.TextureCoordinates.y = attrib.texcoords[2 * index.texcoord_index + 1];
					}

					vertices.push_back(vertex);
				}

				indexOffset += face;
			}			
		}

		Mesh mesh{ vertices, std::make_shared<ColorMaterial>(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}) };

		Logger::Log("finished loading " + path.filename().string() + "!", Logger::Level::Info);

		return mesh;
	}
}