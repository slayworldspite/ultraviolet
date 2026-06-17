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

		Logger::Log("loading textures of type: " + std::to_string(type), Logger::Level::Info);

		for (size_t i{}; i < material->GetTextureCount(type); i++)
		{
			aiString texturePath{};
			material->GetTexture(type, static_cast<unsigned int>(i), &texturePath);

			TextureType textureType{};

			switch (type)
			{
				case aiTextureType_BASE_COLOR:
				case aiTextureType_DIFFUSE:
					textureType = TextureType::Albedo;
					break;

				case aiTextureType_METALNESS:
				case aiTextureType_REFLECTION:
					textureType = TextureType::Metallic;
					break;

				case aiTextureType_DIFFUSE_ROUGHNESS:
				case aiTextureType_SHININESS:
					textureType = TextureType::Roughness;
					break;

				case aiTextureType_NORMAL_CAMERA:
				case aiTextureType_HEIGHT:
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

		// .obj files must have "PBR Extensions" disabled, or it will save anisotropic values, which assimp cannot process
		// it is recommended to keep the pbr extensions and remove the values from the .mtl file manually
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

				auto albedoTextures{ LoadTextures(path, meshMaterial, aiTextureType_BASE_COLOR) };
				textures.insert(textures.end(), albedoTextures.begin(), albedoTextures.end());
			
				// .obj uses aiTextureType_DIFFUSE instead of aiTextureType_BASE_COLOR
				auto diffuseTextures{ LoadTextures(path, meshMaterial, aiTextureType_DIFFUSE) };
				textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

				auto metallicTextures{ LoadTextures(path, meshMaterial, aiTextureType_METALNESS) };
				textures.insert(textures.end(), metallicTextures.begin(), metallicTextures.end());

				// .obj uses aiTextureType_REFLECTION instead of aiTextureType_METALNESS without pbr extensions
				auto reflectionTextures{ LoadTextures(path, meshMaterial, aiTextureType_REFLECTION) };
				textures.insert(textures.end(), reflectionTextures.begin(), reflectionTextures.end());

				auto roughnessTextures{ LoadTextures(path, meshMaterial, aiTextureType_DIFFUSE_ROUGHNESS) };
				textures.insert(textures.end(), roughnessTextures.begin(), roughnessTextures.end());

				// .obj uses aiTextureType_SHININESS instead of aiTextureType_DIFFUSE_ROUGHNESS without pbr extensions
				auto shininessTextures{ LoadTextures(path, meshMaterial, aiTextureType_SHININESS) };
				textures.insert(textures.end(), shininessTextures.begin(), shininessTextures.end());

				auto normalTextures{ LoadTextures(path, meshMaterial, aiTextureType_NORMAL_CAMERA) };
				textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

				// .obj uses aiTextureType_HEIGHT instead of aiTextureType_NORMAL_CAMERA
				auto normalObjTextures{ LoadTextures(path, meshMaterial, aiTextureType_HEIGHT) };
				textures.insert(textures.end(), normalObjTextures.begin(), normalObjTextures.end());

				auto ambientOcclusionTextures{ LoadTextures(path, meshMaterial, aiTextureType_AMBIENT_OCCLUSION) };
				textures.insert(textures.end(), ambientOcclusionTextures.begin(), ambientOcclusionTextures.end());

				material = std::make_shared<TextureMaterial>(textures);
			}
			else
			{
				material = std::make_shared<ColorMaterial>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, 0.5f, 0.5f);
			}

			meshes.emplace_back(vertices, indices, material);
		}

		Logger::Log("finished loading " + path.string() + "!", Logger::Level::Info);

		return Model{ id, meshes };
	}

	
}