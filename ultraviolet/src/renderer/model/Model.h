#pragma once

#include <filesystem>

#include "../mesh/Mesh.h"

namespace ultraviolet
{
	class Model
	{
	public:
		std::vector<Mesh> Meshes;
		glm::vec3 Position;
		glm::vec3 Scale;
		glm::vec3 Rotation;


	public:
		Model(Mesh mesh);

		glm::mat4 GetModelMatrix() const;
	};
}