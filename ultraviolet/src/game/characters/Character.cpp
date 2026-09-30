#include <array>
#include <numeric>

#include <box3d/box3d.h>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Character.h"
#include "../../physics/PhysicsSystem.h"

namespace
{
	struct CollisionPlanesContext
	{
		std::array<::b3CollisionPlane, 16> Planes{};
		size_t Count{};
		bool OnGround{ false };
	};
}

namespace ultraviolet
{
	Character::Character(std::string_view id, glm::vec3 position, float height, float radius) :
		ID{ id },
		Position{ position },
		Velocity{ 0.0f },
		Height{ height },
		OnGround{ true },
		Flying{ false },
		m_Mover{}
	{
		m_Mover.center1 = { 0.0f + position.x, 0.5f + position.y, 0.0f + position.z };
		m_Mover.center2 = { 0.0f + position.x, height + position.y, 0.0f + position.z };
		m_Mover.radius = radius;
	}

	void Character::Update(double deltaTime)
	{
		::b3Pos origin{ ::b3Pos_zero };

		if (!OnGround && !Flying)
			Velocity.y -= Gravity * static_cast<float>(deltaTime);

		if (OnGround && !Flying)
		{
			glm::vec2 horizontalVelocity{ Velocity.x, Velocity.z };

			float speed{ glm::length(horizontalVelocity) };

			if (speed > 0.0f)
			{
				float newSpeed{ std::max(0.0f, speed - Friction * static_cast<float>(deltaTime)) };

				horizontalVelocity *= newSpeed / speed;

				Velocity.x = horizontalVelocity.x;
				Velocity.z = horizontalVelocity.y;
			}
		}

		if (Flying)
		{
			float speed{ glm::length(Velocity) };

			if (speed > 0.0f)
			{
				float newSpeed{ std::max(0.0f, speed - Friction * static_cast<float>(deltaTime)) };

				Velocity *= newSpeed / speed;
			}
		}

		::b3Vec3 velocity{ Velocity.x, Velocity.y, Velocity.z };
		::b3Vec3 translation{ ::b3MulSV(static_cast<float>(deltaTime), velocity) };

		float fraction{ ::b3World_CastMover(
			PhysicsSystem::GetWorldID(),
			origin,
			&m_Mover,
			translation,
			::b3QueryFilter{},
			nullptr,
			nullptr
			)
		};

		::b3Vec3 safeDelta{ ::b3MulSV(fraction, translation) };

		m_Mover.center1 = ::b3Add(m_Mover.center1, safeDelta);
		m_Mover.center2 = ::b3Add(m_Mover.center2, safeDelta);

		CollisionPlanesContext context{};

		::b3World_CollideMover(PhysicsSystem::GetWorldID(), origin, &m_Mover, ::b3DefaultQueryFilter(), PlaneResultCallback, &context);

		OnGround = context.OnGround;
	
		::b3PlaneSolverResult result{ ::b3SolvePlanes(::b3Vec3_zero, context.Planes.data(), static_cast<int>(context.Count)) };

		m_Mover.center1 = ::b3Add(m_Mover.center1, result.delta);
		m_Mover.center2 = ::b3Add(m_Mover.center2, result.delta);

		::b3Vec3 clippedVector = ::b3ClipVector(velocity, context.Planes.data(), static_cast<int>(context.Count));

		Velocity = { clippedVector.x, clippedVector.y, clippedVector.z };

		Position = { m_Mover.center1.x, m_Mover.center1.y, m_Mover.center1.z };
	}

	bool Character::PlaneResultCallback(::b3ShapeId shapeID, const ::b3PlaneResult* plane, int planeCount, void* context)
	{
		CollisionPlanesContext* collisionPlanesContext{ static_cast<CollisionPlanesContext*>(context) };

		for (size_t i{}; i < planeCount; i++)
		{
			if (collisionPlanesContext->Count >= collisionPlanesContext->Planes.size())
				return false;

			::b3CollisionPlane& collisionPlane{ collisionPlanesContext->Planes[collisionPlanesContext->Count] };
			collisionPlanesContext->Count++;

			collisionPlane.plane = plane[i].plane;
			collisionPlane.push = 0.0f;
			collisionPlane.pushLimit = std::numeric_limits<float>::max();
			collisionPlane.clipVelocity = true;

			if (collisionPlane.plane.normal.y > 0.7f)
				collisionPlanesContext->OnGround = true;
		}

		return true;
	}

	void Character::AddToImGui()
	{
		::ImGui::DragFloat3(("Position ##" + ID).c_str(), glm::value_ptr(Position), 0.1f);
		::ImGui::DragFloat3(("Velocity ##" + ID).c_str(), glm::value_ptr(Velocity), 0.1f);
		::ImGui::Text("Height: %.2fm", Height);
		::ImGui::Checkbox(("Flying ##" + ID).c_str(), &Flying);
		::ImGui::DragFloat(("Speed ##" + ID).c_str(), &Speed, 0.1f);
		::ImGui::DragFloat(("Maximum Speed ##" + ID).c_str(), &MaxSpeed, 0.1f);
		::ImGui::DragFloat(("Jump Height ##" + ID).c_str(), &JumpHeight, 0.1f);
		::ImGui::DragFloat(("Gravity ##" + ID).c_str(), &Gravity, 0.01f);
		::ImGui::DragFloat(("Friction ##" + ID).c_str(), &Friction, 0.5f);
		::ImGui::Text("On Ground: %s", OnGround ? "yes" : "no");
	}
}