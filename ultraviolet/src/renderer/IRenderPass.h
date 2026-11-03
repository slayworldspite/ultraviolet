#pragma once

#include "DrawItem.h"

namespace ultraviolet
{
	class IRenderPass
	{
	public:
		virtual ~IRenderPass() = default;

		virtual void Draw(const DrawItem& drawItem) const;
	};
}