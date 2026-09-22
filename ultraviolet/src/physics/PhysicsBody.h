#pragma once

#include <vector>
#include <memory>
#include <string>
#include <string_view>

#include <box3d/id.h>

#include "PhysicsBodyType.h"
#include "../renderer/mesh/Vertex.h"

namespace ultraviolet
{
	class PhysicsBody
	{
	public:
		std::string ID;

		PhysicsBody(std::string_view id, PhysicsBodyType type, float size, const glm::vec3& position, const glm::vec3& rotation, float density = 0.25f, float friction = 0.35f);
		PhysicsBody(std::string_view id, PhysicsBodyType type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, float density = 0.25f, float friction = 0.35f);

		glm::vec3 GetPosition() const;
		glm::quat GetRotation() const;
		void ApplyImpulse(const glm::vec3& impulse);
		void ApplyAngularImpulse(const glm::vec3& impulse);
		void AddToImGui();

	private:
		std::unique_ptr<::b3BodyId> m_BodyID;
		std::unique_ptr<::b3ShapeId> m_ShapeID;
	};
}