#pragma once

#include <glm/gtc/quaternion.hpp>

#include "../mesh/Mesh.h"

namespace ultraviolet
{
	class Model
	{
	public:
		std::string ID;
		std::vector<Mesh> Meshes;
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::vec3 Rotation;
		glm::quat RotationQuaternion;
		bool CastsShadow;
		bool Visible;

	public:
		Model(std::string_view id, std::vector<Mesh>& meshes);

		glm::mat4 GetModelMatrix() const;
		void AddToImGui();
		Mesh* GetMesh(std::string_view id);
	};
}