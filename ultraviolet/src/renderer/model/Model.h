#pragma once

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


	public:
		Model(const std::string& id, std::vector<Mesh>& meshes);

		glm::mat4 GetModelMatrix() const;
		void AddToImGui();
	};
}