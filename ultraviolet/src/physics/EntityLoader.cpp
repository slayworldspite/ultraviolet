#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "EntityLoader.h"
#include "../renderer/model/ModelLoader.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	std::optional<Entity> EntityLoader::Load(const std::string& id, PhysicsBodyType physicsBodyType, const std::filesystem::path& modelPath, const std::filesystem::path& collisionPath, float density, float friction)
	{
		auto renderObject{ ModelLoader::Load("Render Object ##" + id, modelPath) };

		if (!renderObject)
			return {};

		Assimp::Importer importer{};

		const ::aiScene* scene{ importer.ReadFile(
			collisionPath.string(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			//aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph
		) };

		if (scene == nullptr)
		{
			Logger::Log("failed to load " + collisionPath.string() + "!", Logger::Level::Error);
			Logger::Log("assimp: " + std::string{ importer.GetErrorString() }, Logger::Level::Error);

			return {};
		}


		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		size_t vertexCount{};
		size_t faceCount{};

		for (size_t meshIndex{}; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			vertexCount += scene->mMeshes[meshIndex]->mNumVertices;
			faceCount += scene->mMeshes[meshIndex]->mNumFaces;
		}

		vertices.reserve(vertexCount);
		indices.reserve(faceCount * 3);

		for (size_t meshIndex{}; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			auto mesh{ scene->mMeshes[meshIndex] };

			for (size_t vertexIndex{}; vertexIndex < mesh->mNumVertices; vertexIndex++)
			{
				auto& meshVertex{ mesh->mVertices[vertexIndex] };

				Vertex vertex;
				vertex.Position.x = meshVertex.x;
				vertex.Position.y = meshVertex.y;
				vertex.Position.z = meshVertex.z;

				vertices.push_back(vertex);
			}

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
		}

		PhysicsBody physicsBody{ "Physics Object ##" + id, physicsBodyType, vertices, indices, density, friction};

		return Entity{ id, *renderObject, physicsBody };
	}
}