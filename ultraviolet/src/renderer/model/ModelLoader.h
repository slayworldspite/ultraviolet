#pragma once

#include <filesystem>
#include <utility>
#include <string_view>

#include "Model.h"

namespace ultraviolet
{
	namespace ModelLoader
	{
		std::optional<Model> Load(std::string_view id, const std::filesystem::path& modelPath);
	}
}