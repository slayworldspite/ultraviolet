#pragma once

#include <filesystem>

#include "Model.h"

namespace ultraviolet
{
	namespace ModelLoader
	{
		Mesh Load(const std::filesystem::path& path);
	}
}