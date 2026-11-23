#include <glad/glad.h>

#include "SplashScreenRenderer.h"

namespace ultraviolet
{
	SplashScreenRenderer::SplashScreenRenderer(const NativeWindow& window, const std::filesystem::path& splashScreenPath) :
		IRenderer{ window },
		m_ScreenQuad{ Mesh::CreateScreenQuad() },
		m_SplashScreen{ splashScreenPath, TextureType::SplashScreen },
		m_Shader{
			"assets/shaders/splashscreen/SplashScreen.vert",
			"assets/shaders/splashscreen/SplashScreen.frag"
		}
	{

	}

	void SplashScreenRenderer::BeginFrame()
	{
		::glClear(GL_COLOR_BUFFER_BIT);
	}

	void SplashScreenRenderer::EndFrame()
	{
		SwapWindow();
	}

	void SplashScreenRenderer::Execute()
	{
		m_Shader.Use();
		m_Shader.Set("u_SplashScreen", 0);

		m_SplashScreen.Bind();
		m_ScreenQuad.Bind();

		DrawElements(m_ScreenQuad.GetIndexCount());
	}
}