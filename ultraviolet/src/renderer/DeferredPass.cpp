#include "DeferredPass.h"

namespace ultraviolet
{
	DeferredPass::DeferredPass(int width, int height) :
		IRenderPass{
			TextureSettings{
				.Width = width,
				.Height = height,
				.InternalFormat = TextureInternalFormat::Rgba32f,
				.Format = TextureFormat::Rgba,
				.MinFiltering = TextureFilter::Linear,
				.MagFiltering = TextureFilter::Linear,
				.Wrap = TextureWrap::ClampToBorder,
				.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f }
			},
			"assets/shaders/passes/Pass.vert",
			"assets/shaders/passes/Shading.frag"
		}
	{
		
	}

	void DeferredPass::Execute(const RenderPassInfo& info) const
	{
		Shader.Use();

		Shader.Set("u_Diffuse", 0);
		Shader.Set("u_SpecularShininess", 1);
		Shader.Set("u_Normal", 2);
		Shader.Set("u_Position", 3);

		for (const RenderTarget& target : info.ColorAttachments)
			target.Texture->Bind(target.Attachment);

		Output.Bind();
		Draw();
	}
}