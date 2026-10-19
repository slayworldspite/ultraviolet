#pragma once

#include <filesystem>

#include "../../../physics/PhysicsBodyType.h"
#include "../../materials/Material.h"

namespace ultraviolet
{
	struct EntityDescription
	{
		std::string ID;
		PhysicsBodyType Type;
		std::filesystem::path ModelPath;
		std::filesystem::path CollisionModelPath;
		std::shared_ptr<Material> Material;
		float Density = 0.25f;
		float Friction = 0.35f;
	};
}