#pragma once
#include <filesystem>
#include "TGE/Renderer/Texture.h"
namespace TGE
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRenderer();
	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_DocumentIcon;
	};
}


