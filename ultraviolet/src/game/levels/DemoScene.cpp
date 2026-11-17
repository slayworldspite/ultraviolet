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
		scene.AmbientLight.Intensity = 0.0f;
		
		scene.DirectionalLight.Color = glm::vec3{ 1.0f };
		scene.DirectionalLight.Pitch = -65.0f;
		scene.DirectionalLight.Yaw = -125.0f;
		scene.DirectionalLight.Enabled = false;

		scene.Camera.Position.z -= 2.0f;
		scene.Camera.Position.y += 2.0f;
		scene.Camera.Yaw = 90.0f;

		static const std::filesystem::path assets{ "assets" };
		static const std::filesystem::path textures{ assets / "textures" };

		std::shared_ptr<ColorMaterial> material{ std::make_shared<ColorMaterial>(glm::vec3{ 0.35f, 0.60f, 1.0f }, 32) };

		std::vector<EntityDescription> entities{};
		
		entities.push_back(EntityDescription{
			.ID = "ground",
			.Type = PhysicsBodyType::StaticMesh,
			.ModelPath = assets / "scenes/demoscene/map.obj",
			.CollisionModelPath = assets / "scenes/demoscene/map.obj",
			.Material = material
		});

		for (const EntityDescription& entity : entities)
		{
			scene.AddEntity(entity);
		}

		std::random_device device{};
		std::mt19937 generator{};
		std::uniform_real_distribution distribution{ 0.0f, 1.0f };

		int count{ 3 };

		for (int z{ -count }; z <= count; z++)
		{
			for (int x{ -count }; x <= count; x++)
			{
				float r{ distribution(generator) };
				float g{ distribution(generator) };
				float b{ distribution(generator) };

				glm::vec3 color{ r, g, b };

				scene.PointLights.push_back(
					PointLight{
						.ID{ std::format("{}, {}, {}", x, 2, z) },
						.Position{ x * 5.0f, 2.0f, z * 5.0f },
						.Color{ color },
						.Intensity{ 0.01f }
					}
				);
			}
		}
	}
}