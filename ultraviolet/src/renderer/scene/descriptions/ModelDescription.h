#pragma once

#include <filesystem>

#include "../../materials/Material.h"

namespace ultraviolet
{
	struct ModelDescription
	{
		std::string ID;
		std::filesystem::path ModelPath;
		std::shared_ptr<Material> Material;
	};
}