#pragma once

#include "Model.h"

namespace ultraviolet
{
	namespace Primitives
	{
		Model GetPlane(std::string_view id, std::shared_ptr<Material> material = nullptr);
	};
}