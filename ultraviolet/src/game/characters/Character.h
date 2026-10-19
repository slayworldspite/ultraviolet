#pragma once

#include <box3d/types.h>

#include "../../renderer/model/Model.h"

namespace ultraviolet
{
	class Character
	{
	public:
		std::string ID;
		glm::vec3 Position;
		glm::vec2 Throttle;
		float Height;
		bool OnGround;
		bool Flying;
		bool Running;
		float Acceleration{ 30.0f };
		float MinSpeed{ 0.01f };
		float MaxSpeed{ 7.0f };
		float StopSpeed{ 1.0f };
		float JumpHeight{ 5.0f };
		float Gravity{ 15.0f };
		float Friction{ 4.0f };

		Character(std::string_view id, glm::vec3 position = { 0.0f, 0.0f, 0.0f }, float height = 1.8f, float radius = 0.35f);
		virtual ~Character() = default;

		virtual void Update(double deltaTime);

		void AddToImGui();

	private:
		::b3Vec3 m_Velocity;
		::b3Capsule m_Mover;

	private:
		static bool PlaneResultCallback(::b3ShapeId shapeID, const ::b3PlaneResult* plane, int planeCount, void* context);
	};
}