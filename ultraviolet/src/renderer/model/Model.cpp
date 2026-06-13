#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"

namespace ultraviolet
{
	Model::Model(std::vector<Mesh>& meshes) :
		Meshes{ std::move(meshes) },
		Position{},
		Scale{ 1.0 },
		Rotation{}
	{

	}

	glm::mat4 Model::GetModelMatrix() const
	{
		glm::mat4 transform{ 1.0f };

		transform = glm::translate(transform, Position);
		transform = glm::rotate(transform, glm::radians(Rotation.x), glm::vec3{ 1.0f, 0.0f, 0.0f });
		transform = glm::rotate(transform, glm::radians(Rotation.y), glm::vec3{ 0.0f, 1.0f, 0.0f });
		transform = glm::rotate(transform, glm::radians(Rotation.z), glm::vec3{ 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, Scale);

		return transform;
	}
}