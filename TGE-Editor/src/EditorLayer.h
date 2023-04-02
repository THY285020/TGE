#pragma once
#include "TGE.h"
#include "TGE/Core/Layer.h"
#include "SceneHierarchyPanel.h"
namespace TGE
{
	class EditorLayer :public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnImGuiRender() override;
		void OnUpdate(TimeStep& ts) override;
		void OnEvent(Event& event) override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
	private:
		TGE::OrthoCameraController m_CameraController;
		//Temp
		Ref<VertexArray> m_VA;
		Ref<Shader> m_FlatColorShader;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Texture2D> m_Texture;
		glm::vec4 SquareColor = glm::vec4(0.2, 0.3, 0.8, 1.0f);

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_SquareEntity2;
		Entity m_Camera;
		Entity m_Camera2;
		bool m_Primary = true;

		double time_temp = 0.0;
		glm::vec2 m_ViewportSize;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		SceneHierarchyPanel m_SHP;
	};
}

