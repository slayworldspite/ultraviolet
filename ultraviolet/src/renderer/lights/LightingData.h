#pragma once

#include <span>

#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"

namespace ultraviolet
{
	struct LightingData
	{
		const AmbientLight& AmbientLight;
		const DirectionalLight& DirectionalLight;
		const std::vector<PointLight>& PointLights;
	};
}