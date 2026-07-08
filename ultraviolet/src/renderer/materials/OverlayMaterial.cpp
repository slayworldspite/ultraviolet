#include <imgui/imgui.h>

#include "OverlayMaterial.h"

namespace ultraviolet
{
	OverlayMaterial::OverlayMaterial(std::shared_ptr<Texture> overlay) :
		Material{ MaterialType::Overlay },
		Overlay{ overlay }
	{

	}

	OverlayMaterial::~OverlayMaterial()
	{
		Free();
	}

	void OverlayMaterial::Apply() const
	{
		Shader->Set("u_Overlay", 0);

		if (Overlay)
			Overlay->Bind(0);
	}

	void OverlayMaterial::Free()
	{
		if (Overlay)
			Overlay->Delete();
	}

	void OverlayMaterial::AddToImGui(const std::string& id)
	{
		if (::ImGui::CollapsingHeader(("OverlayMaterial ##" + id).c_str()))
		{

			auto overlaySize{ Overlay->GetSize() };
			::ImGui::Image((ImTextureID)(intptr_t)Overlay->GetID(), ImVec2{ static_cast<float>(overlaySize.first), static_cast<float>(overlaySize.second) });

		}
	}
}