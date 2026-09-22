#include <imgui/imgui.h>

#include "Entity.h"

namespace ultraviolet
{
	Entity::Entity(std::string_view id, Model& renderObject, PhysicsBody& physicsObject) :
		ID{ id },
		RenderObject{ std::move(renderObject) },
		PhysicsObject{ std::move(physicsObject) }
	{

	}

	void Entity::Update()
	{
		RenderObject.Position = PhysicsObject.GetPosition();
		RenderObject.RotationQuaternion = PhysicsObject.GetRotation();
	}

	void Entity::AddToImGui()
	{
		if (ImGui::CollapsingHeader(ID.c_str()))
		{
			ImGui::Indent(16.0f);
			RenderObject.AddToImGui();
			PhysicsObject.AddToImGui();
			ImGui::Indent(-16.0f);
		}
	}
}