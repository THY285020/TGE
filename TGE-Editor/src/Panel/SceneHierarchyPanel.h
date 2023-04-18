#pragma once
#include "TGE/Core/core.h"
#include "TGE/Scene/Scene.h"
#include "TGE/Scene/Entity.h"
namespace TGE
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);
		void SetContext(const Ref<Scene>& context);
		void OnImGuiRenderer();
		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0, float columnWidth = 80.0);
		
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
		Entity GetSelectedEntity()const { return m_SelectionContext; }
	//public:
	//	Ref<Texture2D> m_UITexture;
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
		Ref<Texture2D> m_UITexture;
	};
}


