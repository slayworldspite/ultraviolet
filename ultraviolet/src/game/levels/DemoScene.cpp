#include "DemoScene.h"
#include "../../renderer/materials/TextureMaterial.h"

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

		static const std::filesystem::path assets{ "assets" };
		static const std::filesystem::path textures{ assets / "textures" };

		std::shared_ptr<TextureMaterial> material{ std::make_shared<TextureMaterial>(textures / "brick/diffuse.jpg", textures / "brick/rough.jpg", textures / "brick/normal.png", 32) };

		std::vector<EntityDescription> entities{};
		
		entities.push_back(EntityDescription{
			.ID = "floor",
			.Type = PhysicsBodyType::StaticMesh,
			.ModelPath = assets / "scenes/demoscene/map.obj",
			.CollisionModelPath = assets / "scenes/demoscene/map.obj",
			.Material = material
		});

		for (const EntityDescription& entity : entities)
		{
			scene.AddEntity(entity);
		}
	}
}