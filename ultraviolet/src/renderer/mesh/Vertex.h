#pragma once

#include <glm/glm.hpp>

namespace ultraviolet
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normals;
		glm::vec2 TextureCoordinates;
	};
}