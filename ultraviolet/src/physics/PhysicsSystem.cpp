#include <cassert>

#include <box3d/box3d.h>

#include "PhysicsSystem.h"

static bool s_IsInitialized{ false };
static ::b3WorldId s_WorldID{};
static float s_TimeStep{};

namespace ultraviolet
{
	void PhysicsSystem::Initialize(float timeStep)
	{
		::b3WorldDef worldDefinition{ ::b3DefaultWorldDef() };

		s_WorldID = ::b3CreateWorld(&worldDefinition);

		s_TimeStep = timeStep;

		s_IsInitialized = true;
	}

	void PhysicsSystem::Update(double deltaTime)
	{
		assert(s_IsInitialized);

		::b3World_Step(s_WorldID, static_cast<float>(deltaTime), 4);
	}

	::b3WorldId PhysicsSystem::GetWorldID()
	{
		assert(s_IsInitialized);

		return s_WorldID;
	}
	
	void PhysicsSystem::Destroy()
	{
		assert(s_IsInitialized);

		::b3DestroyWorld(s_WorldID);

		s_IsInitialized = false;
	}
}