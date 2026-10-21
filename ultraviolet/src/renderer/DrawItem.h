#pragma once

#include "mesh/Mesh.h"

namespace ultraviolet
{
	struct DrawItem
	{
		const Mesh* Mesh;
		glm::mat4 Transform;
		uint64_t SortKey;

		void GenerateSortKey();
	};
}