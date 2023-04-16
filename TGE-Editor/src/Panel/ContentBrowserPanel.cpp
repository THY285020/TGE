#include "tgpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>

namespace TGE
{
	//Once we have projects, change this
	std::filesystem::path s_Assetpath = "assets";
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_Assetpath)
	{
	}
	void ContentBrowserPanel::OnImGuiRenderer()
	{
		ImGui::Begin("Content Browser");
		//退回上级目录
		if (m_CurrentDirectory != std::filesystem::path(s_Assetpath))
		{
			if (ImGui::Button("..."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		//遍历文件夹目录
		for (auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = it.path();
			auto relativePath = std::filesystem::relative(path, s_Assetpath);//基于asset的相对路径（不显示asset）
			std::string relativePathString = relativePath.filename().string();//filename只显示当前文件名
			//auto p0 = p.path().filename().string();
			//auto p1 = p.path().stem().string();//忽略扩展名，文件夹名的.后面也会忽略
			if (it.is_directory())
			{
				if (ImGui::Button(relativePathString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else//为了显示文件夹内的内容到button
			{
				if (ImGui::Button(relativePathString.c_str()))
				{

				}
			}
			
		}
		ImGui::End();
	}
}