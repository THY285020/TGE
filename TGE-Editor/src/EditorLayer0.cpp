#include "tgpch.h"
#include "EditorLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
//#include "Platform/Opengl/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "TGE/Scene/Serializer.h"
#include "TGE/Utils/PlatformUtils.h"
#include "ImGuizmo.h"
#include "TGE/Math/Math.h"

namespace TGE
{
	float objectMatrix[4][16] = {
  { 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f },

  { 1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  2.f, 0.f, 0.f, 1.f },

  { 1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  2.f, 0.f, 2.f, 1.f },

  { 1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, 2.f, 1.f }
	};

	EditorLayer::EditorLayer() :Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
	{
	}
	void EditorLayer::OnAttach()
	{
		//Texture Init here
		//m_Texture = Texture2D::Create("assets/textures/wood.png");

		TGE::FrameBufferSpecification fbSpec;
		fbSpec.Width = TGE::Application::Get().GetWindow().GetWidth();//TGE::Application::Get().GetWindow().GetWidth();
		fbSpec.Height = TGE::Application::Get().GetWindow().GetHeight();//TGE::Application::Get().GetWindow().GetHeight();
		m_FrameBuffer = TGE::FrameBuffer::Create(fbSpec);

		//Entity
		m_ActiveScene = std::make_shared<Scene>(fbSpec.Width, fbSpec.Height);//创建registry

		m_Camera = m_ActiveScene->CreateEntity("Camera EntityA");
		m_Camera.AddComponent<CameraComponent>();//glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f)
		m_Camera.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.0f));//初始化位置
		m_Camera.GetComponent<CameraComponent>().camera.SetPerspective(glm::radians(45.0f), 0.0f, 1000.f);
#if 0
		//m_SquareEntity = m_ActiveScene->CreateEntity("Square Green");//创建entity
		//m_SquareEntity.AddComponent<TransformComponent>(glm::vec3(-1.f, 0.0f, 0.f));
		//m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0, 1.0, 0.0, 1.0 });

		//m_SquareEntity2 = m_ActiveScene->CreateEntity("Square Red");//创建entity
		//m_SquareEntity2.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.0f, 0.f));
		//m_SquareEntity2.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0, 0.0, 0.0, 1.0 });

		//m_Camera = m_ActiveScene->CreateEntity("Camera EntityA");
		//m_Camera.AddComponent<CameraComponent>();//glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f)
		//m_Camera.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.0f));//初始化位置
		//m_Camera.GetComponent<CameraComponent>().camera.SetPerspective(glm::radians(45.0f), 0.0f, 1000.f);


		//m_Camera2 = m_ActiveScene->CreateEntity("Camera EntityB");
		//auto& cc = m_Camera2.AddComponent<CameraComponent>();//glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f)
		//cc.Primary = false;
		//m_Camera2.AddComponent<TransformComponent>();

		//class CameraController :public ScriptableEntity
		//{
		//public:
		//	void OnCreate()
		//	{
		//		std::cout << "CameraComtroller:OnCreate!" << std::endl;
		//	}
		//	void OnDestroy()
		//	{
		//	}
		//	void OnUpdate(TimeStep ts)
		//	{
		//		auto& translation = GetComponent<TransformComponent>().Translate;

		//		float speed = 5.0f;
		//		if (Input::IsKeyPressed(TGE_KEY_A))
		//			translation.x += speed * ts;
		//		if (Input::IsKeyPressed(TGE_KEY_D))
		//			translation.x -= speed * ts;
		//		if (Input::IsKeyPressed(TGE_KEY_W))
		//			translation.y += speed * ts;
		//		if (Input::IsKeyPressed(TGE_KEY_S))
		//			translation.y -= speed * ts;
		//	}
		//};

		//绑定OnUpdate等函数
		//m_Camera2.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
		m_SHP.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Files"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				//ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				//if (ImGui::MenuItem("Close", NULL, false, dockspaceOpen != NULL))
				//	dockspaceOpen = false;

				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Exit")) 
					TGE::Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
			m_SHP.OnImGuiRenderer();//多级菜单

			ImGui::Begin("Statistics");
				auto stats = TGE::Renderer2D::GetStats();
				ImGui::Text("Renderer2D Stats:");
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads : %d", stats.QuadCount);
				ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
				ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
					//ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");

					//ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");

					//ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");

					//ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
					//if (ImGuizmo::IsUsing())
					//{
					//	ImGui::Text("Using gizmo");
					//}
			ImGui::End();
			//----------------Viewport--------------------

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));//间隔缩小为0
			ImGui::Begin("ViewPort");
			//点击选中或悬停都会更新界面
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

			//Resize此处更新会闪屏
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
			{
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			}

			//uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment();
			//ImGui::Image((void*)m_TextureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			//Gizmo
			//Entity selectedEntity = m_SHP.GetSelectedEntity();
			//m_GizmoType = (ImGuizmo::OPERATION)ImGuizmo::TRANSLATE;
			//if (selectedEntity && m_GizmoType != -1)
			//{
				
			auto& cc = m_Camera.GetComponent<CameraComponent>().camera;
			glm::mat4 cameraProjection = cc.GetProjection();
			glm::mat4 cameraView = glm::lookAt((m_Camera.GetComponent<TransformComponent>().Translate),
				glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.f, 1.f, 0.f));

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::BeginFrame();
				 
				//ImGuizmo::SetDrawlist();

				/*float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);*/

				// Camera
				//auto cameraEntity = m_ActiveScene->GetPrimaryCamera();
				  
				 //auto cameraEntity = m_Camera;
				 //auto& Camera = cameraEntity.GetComponent<CameraComponent>().camera;
				 //Camera.SetViewportSize(io.DisplaySize.x, io.DisplaySize.y);
				 //cameraProjection = Camera.GetProjection();
				 ///*glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());*/
				 //cameraView = glm::lookAt((cameraEntity.GetComponent<TransformComponent>().Translate), 
					// glm::vec3(0.0,0.0,-1.0), glm::vec3(0.f, 1.f, 0.f));
 
				// Entity transform
				//auto& tc = selectedEntity.GetComponent<TransformComponent>();
				//glm::mat4 transform = tc.GetTransform();

				// Snapping
				//bool snap = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL);
				//float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				//// Snap to 45 degrees for rotation
				//if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				//	snapValue = 45.0f;

				//float snapValues[3] = { snapValue, snapValue, snapValue };

				//传入camera的view和projection矩阵，以及transform
				//拖动的时候是true
				//ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				//	(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				//	NULL, snap ? snapValues : NULL);

					EditTransform(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), objectMatrix[0], m_GizmoType);
				 

			//	if (ImGuizmo::IsUsing())
			//	{
			//		glm::vec3 translation, rotation, scale;
			//		Math::DecomposeTransform(transform, translation, rotation, scale);

			//		glm::vec3 deltaRotation = rotation - tc.Rotation;
			//		tc.Translate = translation;
			//		tc.Rotation += deltaRotation;
			//		tc.Scale = scale;

			//		//float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			//		//ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
			//		//glm::vec3 deltaRotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]) - tc.Rotation;
			//		//tc.Translate = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
			//		//tc.Rotation += deltaRotation;
			//		//tc.Scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
			//		//ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(transform));

			//		//glm::vec3 translation, scale, skew;
			//		//glm::quat rotation;
			//		//glm::vec4 perspective;
			//		//glm::decompose(transform, scale, rotation, translation, skew, perspective);
			//		//tc.Rotation = glm::eulerAngles(rotation);
			//		//tc.Translate = translation;
			//		//tc.Scale = scale;
			//	}
			//}

			ImGui::End();
			ImGui::PopStyleVar();
			//----------------Viewport End---------------------
		ImGui::End();

	}

	void EditorLayer::OnUpdate(TimeStep& ts)
	{
		//--------interval--------------
		if (time_temp >= 1.0) {
			TGE_TRACE("fps: {0} ({1}ms)", int(1 / ts.GetTimeSeconds()), ts.GetTimemillSeconds());
			time_temp = 0.0;
		}
		else {
			time_temp += ts.GetTimeSeconds();
		}
		//--------------------Camera----------------
		if(m_ViewportFocused)//没focuse则不更新相机
			m_CameraController.OnUpdate(ts);
		//------------------Resize-----------------
		FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
		if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
		}
		//------------------Renderer---------------
		TGE::Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		//以下是绘制到缓冲的部分
		TGE::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		TGE::RenderCommand::Clear();

		//--------------------Scene----------------
		/*TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());*/
		m_ActiveScene->OnUpdate(ts);
		//TGE::Renderer2D::EndScene();
		//------------------multisquare------------
#if 0
		/*TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());

		static float rotation = 0.0f;
		rotation += 0.002 / ts.GetTimeSeconds();

		TGE::Renderer2D::DrawQuad({ -0.5, -0.5 }, { 0.8, 0.8 }, SquareColor);
		TGE::Renderer2D::DrawQuad({ 0.0, 0.0, -0.1 }, { 10.0, 10.0 }, m_Texture, 10.f);
		TGE::Renderer2D::DrawRotationQuad({ 0.5, -0.5, 0.f }, { 0.75, 0.75 }, glm::vec4(0.8, 0.2, 0.3, 1.0f), -rotation);
		TGE::Renderer2D::DrawRotationQuad({ -1.0, 0.0, 0.0 }, { 1.0, 1.0 }, m_Texture, 1.f, rotation);

		TGE::Renderer2D::EndScene();

		TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.f) / 10.f, 0.4f, (y + 5.0f) / 10.f, 0.5f };
				TGE::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		TGE::Renderer2D::EndScene();*/
#endif

#pragma region Particle
		//Particle
		//if (TGE::Input::IsMosueButtonPressed(TGE_MOUSE_BUTTON_LEFT))
		//{
		//	auto [x, y] = TGE::Input::GetMousePosition();
		//	auto width = TGE::Application::Get().GetWindow().GetWidth();
		//	auto height = TGE::Application::Get().GetWindow().GetHeight();

		//	auto bounds = m_CameraController.GetBounds();//相机边界
		//	auto pos = m_CameraController.GetCamera().GetPosition();//相机位置
		//	x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		//	y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		//	m_Particle.Position = { x + pos.x, y + pos.y };
		//	for (int i = 0; i < 5; i++)
		//		m_ParticleSystem.Emit(m_Particle);
		//}

		//m_ParticleSystem.OnUpdate(ts);
		//m_ParticleSystem.OnRender(m_CameraController.GetCamera());//会开辟一个场景，并渲染粒子系统
#pragma endregion Particle

#pragma region SubTexture
		/*TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());*/

		//SubTextureScene
		//for (uint32_t y = 0; y < m_MapHeight; ++y)
		//{
		//	for (uint32_t x = 0; x < m_MapWidth; ++x)
		//	{
		//		char tileType = s_MapTiles[y * m_MapWidth + x];
		//		TGE::Ref<TGE::SubTexture2D> texture;
		//		if (s_TextureMap.find(tileType) != s_TextureMap.end())
		//		{
		//			texture = s_TextureMap[tileType];
		//		}
		//		else
		//		{
		//			texture = m_BarrelTexture;
		//		}
		//		TGE::Renderer2D::DrawQuad({ x - m_MapWidth/2.0, m_MapHeight-y- m_MapHeight/2.0, 0.0 }, { 1.0, 1.0 }, texture, 1.f);
		//	}
		//}

		//TGE::Renderer2D::DrawQuad({ 0.0, 0.0, 0.0 }, { 1.0, 1.0 }, m_StairTexture, 1.f);
		//TGE::Renderer2D::DrawQuad({ 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, m_BarrelTexture, 1.f);
		//TGE::Renderer2D::DrawQuad({ -1.5, 0.0, 0.0 }, { 1.0, 1.0 }, m_TreeTexture, 1.f);
		//TGE::Renderer2D::DrawQuad({ 1.5, 0.0, 0.0 }, { 1.0, 1.0 }, m_GrassTexture, 1.f);
		//TGE::Renderer2D::EndScene();
#pragma endregion SubTexture

		m_FrameBuffer->UnBind();
	}


	void EditorLayer::OnEvent(TGE::Event& event)
	{
		m_CameraController.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(TGE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

	}

	void EditorLayer::OnDetach()
	{
	}
	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		//shortcuts
		if (e.GetRepeatCount() > 0)
			return false;
		bool control = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL) || Input::IsKeyPressed(TGE_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(TGE_KEY_LEFT_SHIFT) || Input::IsKeyPressed(TGE_KEY_RIGHT_SHIFT);
		switch (e.GetKeyCode())
		{
			case TGE_KEY_N:
			{
				if (control)
				{
					NewScene();
				}
				break;
			}
			case TGE_KEY_O:
			{
				if (control)
				{
					OpenScene();
				}
				break;
			}
			case TGE_KEY_S:
			{
				if (control && shift)
				{
					SaveSceneAs();
				}
				break;
			}
			//Gizmos
			case TGE_KEY_Q:
			{
				m_GizmoType = ImGuizmo::OPERATION::BOUNDS;
				break;
			}
			case TGE_KEY_W:
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case TGE_KEY_E:
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
			case TGE_KEY_R:
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
		}
		return true;
	}
	void EditorLayer::NewScene()
	{
		m_ActiveScene = std::make_shared<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SHP.SetContext(m_ActiveScene);
	}
	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("TGE Scene (*.tge)\0*.tge\0");//传入filter
		if (!filepath.empty())
		{
			m_ActiveScene = std::make_shared<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SHP.SetContext(m_ActiveScene);

			Serializer serializer(m_ActiveScene);
			serializer.DeSerialize(filepath);
		}
	}
	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("TGE Scene (*.tge)\0*.tge\0");
		if (!filepath.empty())
		{
			Serializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
	void EditorLayer::EditTransform(float* cameraView, float* cameraProjection, float* transform, int& m_GizmoType)
	{
		//ImGuizmo::Enable(true);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		//ImGuizmo::SetDrawlist();
		////ImGuiIO& io = ImGui::GetIO();
		////float viewManipulateRight = io.DisplaySize.x;
		////float viewManipulateTop = 0;
		////static ImGuiWindowFlags gizmoWindowFlags = 0;
		//
		//// Snapping
		//bool snap = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL);
		//float snapValue = 0.5f; // Snap to 0.5m for translation/scale
		//// Snap to 45 degrees for rotation
		//if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
		//	snapValue = 45.0f;
		//float snapValues[3] = { snapValue, snapValue, snapValue };

		//float windowWidth = (float)ImGui::GetWindowWidth();
		//float windowHeight = (float)ImGui::GetWindowHeight();
		//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		//ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], 1);
		//ImGuizmo::Manipulate(cameraView, cameraProjection,
		//	(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, transform,
		//	NULL, snap ? snapValues : NULL);

		if (ImGui::RadioButton("Translate", m_GizmoType == ImGuizmo::TRANSLATE))
			m_GizmoType = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_GizmoType == ImGuizmo::ROTATE))
			m_GizmoType = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_GizmoType == ImGuizmo::SCALE))
			m_GizmoType = ImGuizmo::SCALE;

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(transform, matrixTranslation, matrixRotation, matrixScale);
		//ImGui::DragFloat3("Tr", matrixTranslation);
		//ImGui::DragFloat3("Rt", matrixRotation);
		//ImGui::DragFloat3("Sc", matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, transform);

		ImGuiIO& io = ImGui::GetIO();
		float viewManipulateRight = io.DisplaySize.x;
		float viewManipulateTop = 0;
		static ImGuiWindowFlags gizmoWindowFlags = 0;

		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

		//ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
		ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], 1);
		ImGuizmo::Manipulate(cameraView, cameraProjection, (ImGuizmo::OPERATION)m_GizmoType, mCurrentGizmoMode, transform);
		ImGuizmo::ViewManipulate(cameraView, 8.f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
		if (ImGuizmo::IsUsing())
		{

		}
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
}
