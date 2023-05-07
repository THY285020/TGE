#include "tgpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>

namespace TGE
{
	//Once we have projects, change this
	extern const std::filesystem::path s_Assetpath = "assets";
	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_Assetpath)
	{
		m_DirectoryIcon = Texture2D::Create("assets/Icons/folderW.png");
		m_DocumentIcon = Texture2D::Create("assets/Icons/docsW.png");
	}
	void ContentBrowserPanel::OnImGuiRenderer()
	{
		ImGui::Begin("Content Browser");
		//�˻��ϼ�Ŀ¼
		if (m_CurrentDirectory != s_Assetpath)//path(s_Assetpath)
		{
			if (ImGui::Button("..."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float iconSize = 64.f;
		float cellSize = iconSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnNum = (int)(panelWidth / cellSize);
		if (columnNum < 1) columnNum = 1;
		ImGui::Columns(columnNum, 0, false);

		//�����ļ���Ŀ¼
		for (auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = it.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());//id����������֣����������ַ���������ָ�룬�˴�Push��������ʹ��ͬһ�������ImageButton
			//auto relativePath = std::filesystem::relative(path, s_Assetpath);//����asset�����·��������ʾasset��
			//std::string filenameString = relativePath.filename().string();//filenameֻ��ʾ��ǰ�ļ���
			//auto p0 = p.path().filename().string();
			//auto p1 = p.path().stem().string();//������չ�����ļ�������.����Ҳ�����
			Ref<Texture2D> icon = it.is_directory() ? m_DirectoryIcon : m_DocumentIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { iconSize, iconSize }, { 0,1 }, { 1,0 });
			//�϶���Դ
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = path.c_str();//relativePath
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1)*sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();
			//���������Ŀ¼
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (it.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::PopID();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::SliderFloat("iconSize", &iconSize, 16, 512);
		ImGui::SliderFloat("padding", &padding, 0, 32);

		ImGui::End();
	}
}