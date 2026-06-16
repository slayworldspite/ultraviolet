#pragma once

#include <filesystem>
#include <utility>

#include "Model.h"

namespace ultraviolet
{
	namespace ModelLoader
	{
		std::optional<Model> Load(const std::string& id, const std::filesystem::path& path);
	}
}