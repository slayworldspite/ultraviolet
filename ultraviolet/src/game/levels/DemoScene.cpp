#include <random>

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
		scene.DirectionalLight.Enabled = true;

		scene.Camera.Position = { 3.0f, 2.0f, -20.0f };
		scene.Camera.Yaw = 90.0f;

		static const std::filesystem::path assets{ "assets" };
		static const std::filesystem::path textures{ assets / "textures" };

		std::shared_ptr<ColorMaterial> material{ std::make_shared<ColorMaterial>(glm::vec3{ 1.0f, 1.0f, 1.0f }, 32) };

		std::vector<EntityDescription> entities{};
		
		entities.push_back(EntityDescription{
			.ID = "ground",
			.Type = PhysicsBodyType::StaticMesh,
			.ModelPath = assets / "scenes/demoscene/mh.obj",
			.CollisionModelPath = assets / "scenes/demoscene/mh.obj",
			.Material = material
		});

		for (const EntityDescription& entity : entities)
		{
			scene.AddEntity(entity);
		}
	}
}