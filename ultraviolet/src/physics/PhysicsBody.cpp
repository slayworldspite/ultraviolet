#include <exception>

#include <box3d/box3d.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "PhysicsBody.h"
#include "PhysicsSystem.h"
#include "../utils/Logger.h"

namespace ultraviolet
{
	PhysicsBody::PhysicsBody(std::string_view id, PhysicsBodyType type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, float density, float friction) :
		ID{ id },
		m_BodyID{ nullptr },
		m_ShapeID{ nullptr }
	{
		::b3BodyDef bodyDefinition{ ::b3DefaultBodyDef() };

		switch (type)
		{
			case PhysicsBodyType::StaticMesh:
			case PhysicsBodyType::StaticConvexHull:
				bodyDefinition.type = ::b3_staticBody;
				break;

			case PhysicsBodyType::DynamicConvexHull:
				bodyDefinition.type = ::b3_dynamicBody;
				break;

			default:
				bodyDefinition.type = ::b3_staticBody;
				break;
		}

		m_BodyID = std::make_unique<::b3BodyId>(::b3CreateBody(PhysicsSystem::GetWorldID(), &bodyDefinition));

		std::vector<::b3Vec3> vertexPositions{};
		vertexPositions.reserve(vertices.size());

		for (const Vertex& vertex : vertices)
		{
			const glm::vec3& position{ vertex.Position };
			vertexPositions.push_back({ position.x, position.y, position.z });
		}

		std::vector<int32_t> box3dIndices{};
		box3dIndices.reserve(indices.size());

		for (uint32_t index : indices)
		{
			box3dIndices.push_back(static_cast<int32_t>(index));
		}

		::b3MeshDef meshDefinition{};

		meshDefinition.vertices = vertexPositions.data();
		meshDefinition.vertexCount = static_cast<int>(vertexPositions.size());

		meshDefinition.indices = box3dIndices.data();
		meshDefinition.triangleCount = static_cast<int>(box3dIndices.size() / 3);

		meshDefinition.weldVertices = true;
		meshDefinition.identifyEdges = true;

		int size{ static_cast<int>(vertexPositions.size()) };

		if (type == PhysicsBodyType::StaticMesh)
		{
			::b3MeshData* meshData{ ::b3CreateMesh(&meshDefinition, nullptr, 0) };

			if (!meshData)
			{
				Logger::Log("failed to create mesh!", Logger::Level::Critical);
				Logger::Log(id, Logger::Level::Error);

				throw std::runtime_error{ "failed to create mesh!" };
			}

			::b3ShapeDef boxShapeDefinition{ ::b3DefaultShapeDef() };
			boxShapeDefinition.density = density;
			boxShapeDefinition.baseMaterial.friction = friction;

			m_ShapeID = std::make_unique<::b3ShapeId>(::b3CreateMeshShape(*m_BodyID, &boxShapeDefinition, meshData, ::b3Vec3_one));
		}
		else
		{
			::b3HullData* hullData{ ::b3CreateHull(vertexPositions.data(), size, size) };

			if (!hullData)
			{
				Logger::Log("failed to create hull!", Logger::Level::Critical);
				Logger::Log(id, Logger::Level::Error);
				Logger::Log("degenerate input: coincident or coplanar points, or insufficient volume", Logger::Level::Error);

				throw std::runtime_error{ "failed to create hull! degenerate input: coincident or coplanar points, or insufficient volume" };
			}

			::b3ShapeDef boxShapeDefinition{ ::b3DefaultShapeDef() };
			boxShapeDefinition.density = density;
			boxShapeDefinition.baseMaterial.friction = friction;

			m_ShapeID = std::make_unique<::b3ShapeId>(::b3CreateHullShape(*m_BodyID, &boxShapeDefinition, hullData));
		}
	}

	glm::vec3 PhysicsBody::GetPosition() const
	{
		if (m_BodyID)
		{
			auto position{ ::b3Body_GetPosition(*m_BodyID) };

			return glm::vec3{ position.x, position.y, position.z };
		}
		return glm::vec3{ 0.0f, 0.0f, 0.0f };
	}

	glm::quat PhysicsBody::GetRotation() const
	{
		if (m_BodyID)
		{
			auto rotation{ ::b3Body_GetRotation(*m_BodyID) };

			return glm::quat{ rotation.s, rotation.v.x, rotation.v.y, rotation.v.z };
		}
		return glm::quat{ 0.0f, 0.0f, 0.0f, 0.0f };
	}

	void PhysicsBody::ApplyImpulse(const glm::vec3& impulse)
	{
		if (m_BodyID)
		{
			::b3Vec3 box3dImpulse{ impulse.x, impulse.y, impulse.z };

			::b3Body_ApplyLinearImpulseToCenter(*m_BodyID, box3dImpulse, true);
		}
	}

	void PhysicsBody::ApplyAngularImpulse(const glm::vec3& impulse)
	{
		if (m_BodyID)
		{
			::b3Vec3 box3dImpulse{ impulse.x, impulse.y, impulse.z };

			::b3Body_ApplyAngularImpulse(*m_BodyID, box3dImpulse, true);
		}
	}

	void PhysicsBody::AddToImGui()
	{
		if (ImGui::CollapsingHeader(ID.c_str()))
		{
			glm::vec3 position{ GetPosition() };
			auto velocity{ ::b3Body_GetLinearVelocity(*m_BodyID) };

			ImGui::DragFloat3("Position", glm::value_ptr(position));
			ImGui::DragFloat3("Velocity", &velocity.x);
		}
	}
}