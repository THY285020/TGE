#pragma once
#include "TGE.h"
#include "TGE/Core/Layer.h"
#include "TGE/Renderer/EditorCamera.h"
#include "Panel/SceneHierarchyPanel.h"
#include "Panel/ContentBrowserPanel.h"
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
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void EditTransform(float* cameraView, float* cameraProjection, float* transform, int& m_GizmoType);
		void UI_Toolbar();

		void OnScenePlay();
		void OnSceneStop();

		void DuplicateEntity();
		//void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
	private:
		OrthoCameraController m_CameraController;
		//Temp
		Ref<VertexArray> m_VA;
		Ref<Shader> m_FlatColorShader;
		Ref<FrameBuffer> m_FrameBuffer;

		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_IconPlay;
		Ref<Texture2D> m_IconStop;
		glm::vec4 SquareColor = glm::vec4(0.2, 0.3, 0.8, 1.0f);

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		Entity m_SquareEntity;
		Entity m_SquareEntity2;
		Entity m_Camera;
		Entity m_Camera2;
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;
		bool m_Primary = true;

		double time_temp = 0.0;
		glm::vec2 m_ViewportSize;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		SceneHierarchyPanel m_SHP;
		ContentBrowserPanel m_CBP;

		int m_GizmoType = (1u << 0)| (1u << 1)| (1u << 2);
		int m_lastUsing = 0;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;
	};
}


