#pragma once

#include "../renderer/model/Model.h"
#include "PhysicsBody.h"

namespace ultraviolet
{
	class Entity
	{
	public:
		std::string ID;
		Model RenderObject;
		PhysicsBody PhysicsObject;

	public:
		Entity(std::string_view id, Model& renderObject, PhysicsBody& physicsObject);

		void Update();
		void AddToImGui();
	};
}