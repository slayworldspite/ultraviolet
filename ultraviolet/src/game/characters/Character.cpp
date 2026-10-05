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
		Throttle{ 0.0f },
		Height{ height },
		OnGround{ true },
		Flying{ false },
		Running{ false},
		m_Velocity{ 0.0f },
		m_Mover{}
	{
		m_Mover.center1 = { 0.0f + position.x, 0.5f + position.y, 0.0f + position.z };
		m_Mover.center2 = { 0.0f + position.x, height + position.y, 0.0f + position.z };
		m_Mover.radius = radius;
	}

	void Character::Update(double deltaTime, const glm::vec3& forward, const glm::vec3& right)
	{
		float fDeltaTime{ static_cast<float>(deltaTime) };

		::b3Pos origin{ ::b3Pos_zero };

		float speed{ ::b3Length(m_Velocity) };

		if (speed < MinSpeed)
		{
			m_Velocity.x = 0.0f;
			m_Velocity.z = 0.0f;
		}
		else
		{
			if (OnGround || Flying)
			{
				if (speed > 0.0f)
				{
					float control{ speed < StopSpeed ? StopSpeed : speed };

					float drop{ control * Friction * fDeltaTime };
					float newSpeed{ std::max(0.0f, speed - drop) };

					float ratio{ newSpeed / speed };

					m_Velocity.x *= ratio;
					m_Velocity.z *= ratio;
				}
			}
		}

		float maxSpeed{ Running ? MaxSpeed * 1.5f : MaxSpeed };

		::b3Vec3 b3Forward{ forward.x, forward.y, forward.z };
		::b3Vec3 b3Right{ right.x, right.y, right.z };

		::b3Vec3 desiredVelocity{ maxSpeed * Throttle.x * b3Forward + maxSpeed * Throttle.y * b3Right };

		float desiredSpeed;
		::b3Vec3 desiredDirection{ ::b3GetLengthAndNormalize(&desiredSpeed, desiredVelocity) };

		if (desiredSpeed > maxSpeed)
		{
			desiredVelocity *= maxSpeed / desiredSpeed;
			desiredSpeed = maxSpeed;
		}

		if (OnGround)
			m_Velocity.y = 0.0f;

		float currentSpeed{ ::b3Dot(m_Velocity, desiredDirection) };
		float addSpeed{ desiredSpeed - currentSpeed };

		if (addSpeed > 0.0f)
		{
			float acceleration{ Acceleration * maxSpeed * fDeltaTime };

			if (acceleration > addSpeed)
			{
				acceleration = addSpeed;
			}

			m_Velocity += acceleration * desiredDirection;
		}

		if (!Flying)
			m_Velocity.y -= Gravity * fDeltaTime;

		::b3Vec3 translation{ ::b3MulSV(fDeltaTime, m_Velocity) };

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

		m_Velocity = ::b3ClipVector(m_Velocity, context.Planes.data(), static_cast<int>(context.Count));

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
		::ImGui::DragFloat3(("Velocity ##" + ID).c_str(), &m_Velocity.x, 0.1f);
		::ImGui::Text("Height: %.2fm", Height);
		::ImGui::Checkbox(("Flying ##" + ID).c_str(), &Flying);
		::ImGui::DragFloat(("Acceleration ##" + ID).c_str(), &Acceleration, 0.1f);
		::ImGui::DragFloat(("Minimum Speed ##" + ID).c_str(), &MinSpeed, 0.1f);
		::ImGui::DragFloat(("Maximum Speed ##" + ID).c_str(), &MaxSpeed, 0.1f);
		::ImGui::DragFloat(("Stop Speed ##" + ID).c_str(), &StopSpeed, 0.1f);
		::ImGui::DragFloat(("Jump Height ##" + ID).c_str(), &JumpHeight, 0.1f);
		::ImGui::DragFloat(("Gravity ##" + ID).c_str(), &Gravity, 0.01f);
		::ImGui::DragFloat(("Friction ##" + ID).c_str(), &Friction, 0.5f);
		::ImGui::Text("On Ground: %s", OnGround ? "yes" : "no");
	}
}