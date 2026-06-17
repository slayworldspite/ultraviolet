#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelLoader.h"
#include "../../utils/Logger.h"

namespace ultraviolet
{
	std::vector<std::shared_ptr<Texture>> LoadTextures(const std::filesystem::path& path, aiMaterial*& material, aiTextureType type)
	{
		std::vector<std::shared_ptr<Texture>> textures{};

		for (size_t i{}; i < material->GetTextureCount(type); i++)
		{
			aiString texturePath{};
			material->GetTexture(type, static_cast<unsigned int>(i), &texturePath);

			TextureType textureType{};

			switch (type)
			{
				case aiTextureType_BASE_COLOR:
					textureType = TextureType::Albedo;
					break;

				case aiTextureType_METALNESS:
					textureType = TextureType::Metallic;
					break;

				case aiTextureType_DIFFUSE_ROUGHNESS:
					textureType = TextureType::Roughness;
					break;

				case aiTextureType_NORMAL_CAMERA:
					textureType = TextureType::Normal;
					break;

				case aiTextureType_AMBIENT_OCCLUSION:
					textureType = TextureType::AmbientOcclusion;
					break;
			}

			std::shared_ptr<Texture> texture{ std::make_shared<Texture>(path.parent_path().string() + "/" + texturePath.C_Str(), textureType) };
			textures.push_back(texture);
		}

		return textures;
	}

	std::optional<Model> ModelLoader::Load(const std::string& id, const std::filesystem::path& path)
	{
		Logger::Log("loading " + path.string(), Logger::Level::Info);

		::Assimp::Importer importer{};

		const ::aiScene* scene{ importer.ReadFile(
			path.string(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph
		) };

		if (scene == nullptr)
		{
			Logger::Log("failed to load " + path.string() + "!", Logger::Level::Error);
			Logger::Log("assimp: " + std::string{ importer.GetErrorString() }, Logger::Level::Error);

			return {};
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

				auto& meshNormal{ mesh->mNormals[vertexIndex] };

				vertex.Normal.x = meshNormal.x;
				vertex.Normal.y = meshNormal.y;
				vertex.Normal.z = meshNormal.z;

				auto& meshTangent{ mesh->mTangents[vertexIndex] };

				vertex.Tangent.x = meshTangent.x;
				vertex.Tangent.y = meshTangent.y;
				vertex.Tangent.z = meshTangent.z;
				
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

			std::shared_ptr<IMaterial> material;

			if (mesh->mMaterialIndex >= 0)
			{
				auto& meshMaterial{ scene->mMaterials[mesh->mMaterialIndex] };

				std::vector<std::shared_ptr<Texture>> textures{};

				auto diffuseTextures{ LoadTextures(path, meshMaterial, aiTextureType_DIFFUSE) };
				textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

				auto specularTextures{ LoadTextures(path, meshMaterial, aiTextureType_SPECULAR) };
				textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

				auto normalsTextures{ LoadTextures(path, meshMaterial, aiTextureType_NORMALS) };
				textures.insert(textures.end(), normalsTextures.begin(), normalsTextures.end());

				material = std::make_shared<TextureMaterial>(textures);
			}
			else
			{
				material = std::make_shared<ColorMaterial>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, 32);
			}

			meshes.emplace_back(vertices, indices, material);
		}

		Logger::Log("finished loading " + path.string() + "!", Logger::Level::Info);

		return Model{ id, meshes };
	}

	
}