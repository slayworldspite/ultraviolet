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
		std::shared_ptr<ColorMaterial> material{ std::make_shared<ColorMaterial>(glm::vec3{1.0f, 0.0f, 1.0f}, 64) };

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