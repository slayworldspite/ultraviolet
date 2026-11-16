#pragma once

#include "IRenderPass.h"

namespace ultraviolet
{
	class DeferredPass : public IRenderPass
	{
	public:
		DeferredPass(int width, int height);
		virtual ~DeferredPass() = default;

		virtual void Execute(const RenderPassInfo& info) const override;
	};
}