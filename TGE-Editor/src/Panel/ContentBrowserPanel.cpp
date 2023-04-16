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
		//�˻��ϼ�Ŀ¼
		if (m_CurrentDirectory != std::filesystem::path(s_Assetpath))
		{
			if (ImGui::Button("..."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		//�����ļ���Ŀ¼
		for (auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = it.path();
			auto relativePath = std::filesystem::relative(path, s_Assetpath);//����asset�����·��������ʾasset��
			std::string relativePathString = relativePath.filename().string();//filenameֻ��ʾ��ǰ�ļ���
			//auto p0 = p.path().filename().string();
			//auto p1 = p.path().stem().string();//������չ�����ļ�������.����Ҳ�����
			if (it.is_directory())
			{
				if (ImGui::Button(relativePathString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else//Ϊ����ʾ�ļ����ڵ����ݵ�button
			{
				if (ImGui::Button(relativePathString.c_str()))
				{

				}
			}
			
		}
		ImGui::End();
	}
}