#pragma once

#include "IRenderPass.h"

namespace ultraviolet
{
	class DeferredPass : public IRenderPass
	{
	public:
		DeferredPass(int width, int height);
		DeferredPass(const DeferredPass&) = delete;
		DeferredPass& operator=(const DeferredPass&) = delete;
		DeferredPass(DeferredPass&& other) noexcept = default;
		DeferredPass& operator=(DeferredPass&& other) noexcept = default;
		virtual ~DeferredPass() = default;

		virtual void Execute(const RenderPassInfo& info) const override;
	};
}