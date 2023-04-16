#pragma once
#include <filesystem>
namespace TGE
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRenderer();
	private:
		std::filesystem::path m_CurrentDirectory;
	};
}


