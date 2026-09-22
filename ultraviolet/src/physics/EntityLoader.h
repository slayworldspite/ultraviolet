#pragma once

#include "Entity.h"

namespace ultraviolet
{
	namespace EntityLoader
	{
		std::optional<Entity> Load(const std::string& id, PhysicsBodyType physicsBodyType, const std::filesystem::path& modelPath, const std::filesystem::path& collisionPath, float density = 0.25f, float friction = 0.35f);
	}
}