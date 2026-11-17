#pragma once

#include "mesh/Mesh.h"

namespace ultraviolet
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;

		virtual void BeginFrame() const = 0;
		virtual void EndFrame() const = 0;
		virtual void Execute() = 0;

	protected:
		void BindBackbuffer() const;
		void DrawElements(size_t indexCount) const;
		static Mesh CreateScreenQuad();
	};
}