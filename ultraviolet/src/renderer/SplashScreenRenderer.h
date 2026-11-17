#pragma once

#include "IRenderer.h"
#include "objects/Texture.h"

namespace ultraviolet
{
	class SplashScreenRenderer : public IRenderer
	{
	public:
		SplashScreenRenderer(const NativeWindow& window, const std::filesystem::path& splashScreenPath);
		virtual ~SplashScreenRenderer() = default;

		virtual void BeginFrame() const override;
		virtual void EndFrame() const override;
		virtual void Execute() override;

	private:
		Mesh m_ScreenQuad;
		Shader m_Shader;
		Texture m_SplashScreen;
	};
}