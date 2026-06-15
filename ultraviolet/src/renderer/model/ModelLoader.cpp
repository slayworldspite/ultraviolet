#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelLoader.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	Model ModelLoader::Load(const std::string& id, const std::filesystem::path& path)
	{
		Logger::Log("loading " + path.string(), Logger::Level::Info);

		::Assimp::Importer importer{};

		const ::aiScene* scene{ importer.ReadFile(
			path.string(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenSmoothNormals |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph
		) };

		if (scene == nullptr)
		{
			Logger::Log("failed to load " + path.string() + "!", Logger::Level::Critical);
			Logger::Log("assimp: " + std::string{ importer.GetErrorString() }, Logger::Level::Error);

			// todo: don't exit
			exit(-1);
		}

		std::vector<Mesh> meshes;
		meshes.reserve(scene->mNumMeshes);

		for (size_t meshIndex{}; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			auto mesh{ scene->mMeshes[meshIndex] };

			std::vector<Vertex> vertices;
			vertices.reserve(mesh->mNumVertices);

			for (size_t vertexIndex{}; vertexIndex < mesh->mNumVertices; vertexIndex++)
			{
				auto& meshVertex{ mesh->mVertices[vertexIndex] };
				
				Vertex vertex;
				vertex.Position.x = meshVertex.x;
				vertex.Position.y = meshVertex.y;
				vertex.Position.z = meshVertex.z;

				if (mesh->HasNormals())
				{
					auto& meshNormals{ mesh->mNormals[vertexIndex] };

					vertex.Normals.x = meshNormals.x;
					vertex.Normals.y = meshNormals.y;
					vertex.Normals.z = meshNormals.z;
				}
				
				if (mesh->HasTextureCoords(0) && mesh->mTextureCoords[0])
				{
					vertex.TextureCoordinates.x = mesh->mTextureCoords[0][vertexIndex].x;
					vertex.TextureCoordinates.y = mesh->mTextureCoords[0][vertexIndex].y;
				}
				else
				{
					vertex.TextureCoordinates.x = 0.0f;
					vertex.TextureCoordinates.y = 0.0f;
				}

				vertices.push_back(vertex);
			}

			std::vector<uint32_t> indices;
			indices.reserve(mesh->mNumFaces * 3);

			for (size_t faceIndex{}; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				auto& face{ mesh->mFaces[faceIndex] };

				if (face.mNumIndices != 3)
					Logger::Log("more than 3 indices per face!", Logger::Level::Warning);

				for (size_t indexIndex{}; indexIndex < face.mNumIndices; indexIndex++)
				{
					indices.push_back(face.mIndices[indexIndex]);
				}
			}

			meshes.emplace_back(vertices, indices, std::make_shared<ColorMaterial>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }));
		}

		Logger::Log("finished loading " + path.string() + "!", Logger::Level::Info);

		return Model{ id, meshes };
	}
}