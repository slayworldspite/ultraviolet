#pragma once

struct b3WorldId;

namespace ultraviolet
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem() = delete;

		static void Initialize();
		static void Update(double deltaTime);
		static ::b3WorldId GetWorldID();
		static void Destroy();
	};
}