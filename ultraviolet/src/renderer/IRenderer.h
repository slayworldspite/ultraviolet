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
		void BindBackbuffer();
		void DrawElements(size_t indexCount);
		static Mesh CreateScreenQuad();
	};
}