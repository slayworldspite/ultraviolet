#include "DemoScene.h"
#include "../../renderer/materials/ColorMaterial.h"

namespace ultraviolet
{
	DemoScene::DemoScene() :
		Level{ "DemoScene" }
	{

	}

	void DemoScene::Load(Scene& scene)
	{
		scene.AmbientLight.Color = glm::vec3{ 1.0f };
		scene.AmbientLight.Intensity = 0.1f;
		
		scene.DirectionalLight.Color = glm::vec3{ 1.0f };
		scene.DirectionalLight.Pitch = -65.0f;
		scene.DirectionalLight.Yaw = -125.0f;

		std::vector<EntityDescription> entities{};
		
		entities.push_back(EntityDescription{
			.ID = "floor",
			.Type = PhysicsBodyType::StaticMesh,
			.ModelPath = "assets/scenes/demoscene/floor.obj",
			.CollisionModelPath = "assets/scenes/demoscene/floor.obj",
			.Material = material
		});
		
		entities.push_back(EntityDescription{
			.ID = "wall",
			.Type = PhysicsBodyType::StaticMesh,
			.ModelPath = "assets/scenes/demoscene/wall.obj",
			.CollisionModelPath = "assets/scenes/demoscene/wall.obj",
			.Material = material
		});

		for (const EntityDescription& entity : entities)
		{
			scene.AddEntity(entity);
		}
	}
}