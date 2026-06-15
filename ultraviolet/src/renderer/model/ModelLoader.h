#pragma once

#include <filesystem>

#include "Model.h"

namespace ultraviolet
{
	namespace ModelLoader
	{
		Model Load(const std::string& id, const std::filesystem::path& path);
	}
}