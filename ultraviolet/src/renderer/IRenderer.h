#pragma once

#include "../core/NativeWindow.h"
#include "mesh/Mesh.h"

namespace ultraviolet
{
	class IRenderer
	{
	public:
		IRenderer(const NativeWindow& window);
		virtual ~IRenderer() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Execute() = 0;

	protected:
		void BindBackbuffer() const;
		void DrawElements(size_t indexCount) const;
		void SwapWindow() const;
		::SDL_Window* GetNativeHandle();
		static Mesh CreateScreenQuad();

	private:
		::SDL_Window* m_Window;
	};
}