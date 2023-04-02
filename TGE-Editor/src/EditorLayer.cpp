#include "tgpch.h"
#include "EditorLayer.h"
#include "imgui/imgui.h"
//#include "Platform/Opengl/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	EditorLayer::EditorLayer() :Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
	{
	}
	void EditorLayer::OnAttach()
	{
		//Texture Init here
		m_Texture = Texture2D::Create("assets/textures/wood.png");

		TGE::FrameBufferSpecification fbSpec;
		fbSpec.Width = TGE::Application::Get().GetWindow().GetWidth();//TGE::Application::Get().GetWindow().GetWidth();
		fbSpec.Height = TGE::Application::Get().GetWindow().GetHeight();//TGE::Application::Get().GetWindow().GetHeight();
		m_FrameBuffer = TGE::FrameBuffer::Create(fbSpec);

		//Entity
		m_ActiveScene = std::make_shared<Scene>(fbSpec.Width, fbSpec.Height);//创建registry

		m_SquareEntity = m_ActiveScene->CreateEntity("Square Green");//创建entity
		m_SquareEntity.AddComponent<TransformComponent>(glm::vec3(-1.f, 0.0f, 0.f));
		m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0, 1.0, 0.0, 1.0 });

		m_SquareEntity2 = m_ActiveScene->CreateEntity("Square Red");//创建entity
		m_SquareEntity2.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.0f, 0.f));
		m_SquareEntity2.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0, 0.0, 0.0, 1.0 });

		m_Camera = m_ActiveScene->CreateEntity("Camera EntityA");
		m_Camera.AddComponent<CameraComponent>();//glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f)
		m_Camera.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.0f));//初始化位置
		m_Camera.GetComponent<CameraComponent>().camera.SetPerspective(glm::radians(45.0f), 0.0f, 1000.f);


		m_Camera2 = m_ActiveScene->CreateEntity("Camera EntityB");
		auto& cc = m_Camera2.AddComponent<CameraComponent>();//glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f)
		cc.Primary = false;
		m_Camera2.AddComponent<TransformComponent>();

		class CameraController :public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				std::cout << "CameraComtroller:OnCreate!" << std::endl;
			}
			void OnDestroy()
			{
			}
			void OnUpdate(TimeStep ts)
			{
				auto& translation = GetComponent<TransformComponent>().Translate;

				float speed = 5.0f;
				if (Input::IsKeyPressed(TGE_KEY_A))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(TGE_KEY_D))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(TGE_KEY_W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(TGE_KEY_S))
					translation.y -= speed * ts;
			}
		};

		//绑定OnUpdate等函数
		m_Camera2.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SHP.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockingEnable = true;
		if (dockingEnable)
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
			//ImGuiStyle& style = ImGui::GetStyle();
			//float WinMinSizeX = style.WindowMinSize.x;
			//style.WindowMinSize.x = 370.f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			//style.WindowMinSize.x = WinMinSizeX;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Options"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
					ImGui::MenuItem("Padding", NULL, &opt_padding);
					ImGui::Separator();//分割线

					if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
					if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
					if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
					if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
					if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
					ImGui::Separator();

					//if (ImGui::MenuItem("Close", NULL, false, dockspaceOpen != NULL))
					//	dockspaceOpen = false;
					if (ImGui::MenuItem("Exit")) TGE::Application::Get().Close();
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			m_SHP.OnImGuiRenderer();//多级菜单

			//--------------------------Statistics菜单
			ImGui::Begin("Statistics");

			auto stats = TGE::Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads : %d", stats.QuadCount);
			ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

			//调整颜色
			//if (m_SquareEntity)
			//{
			//	ImGui::Separator();
			//	auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			//	auto& SquareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
			//	ImGui::Text("%s", tag.c_str());
			//	ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
			//	ImGui::Separator();
			//}
			//transform and CheckBox
			//ImGui::DragFloat3("Camera Transform",
			//	glm::value_ptr(m_Camera.GetComponent<TransformComponent>().Transform[3]));
			//if (ImGui::Checkbox("Camera A", &m_Primary))
			//{
			//	m_Camera2.GetComponent<CameraComponent>().Primary = !m_Primary;
			//	m_Camera.GetComponent<CameraComponent>().Primary = m_Primary;
			//}
			
			//orthoSize
			//{
			//	auto& cc = m_Camera2.GetComponent<CameraComponent>().camera;
			//	float orthoSize = cc.GetOrthographicSize();
			//	if (ImGui::DragFloat("Camera2 OrthoSize", &orthoSize))
			//		cc.SetOrthographicSize(orthoSize);
			//}

			ImGui::End();
			//----------------Statistics End
			//----------------Viewport
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));//间隔缩小为0
			ImGui::Begin("ViewPort");
			//点击选中或悬停都会更新界面
			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

			//Resize此处更新会闪屏
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
			{
				m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			}
			uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment();
			ImGui::Image((void*)m_TextureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::End();
		}
		else
		{
			ImGui::Begin("Settings");

			auto stats = TGE::Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads : %d", stats.QuadCount);
			ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

			ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
			uint32_t m_TextureID = m_Texture->GetRendererID();
			/*uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment();*/
			//将图像传入到setting页面 后俩用于颠倒操作
			ImGui::Image((void*)m_TextureID, ImVec2{ 1280.f, 720.f }, ImVec2{ 0, 1 }, ImVec2{1, 0});

			ImGui::End();
		}
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
		TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());

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
		TGE::Renderer2D::EndScene();
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
	}

	void EditorLayer::OnDetach()
	{
	}
}




