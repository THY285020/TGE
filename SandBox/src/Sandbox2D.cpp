#include "tgpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
//#include "Platform/Opengl/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles = 
"wwwwwwwwwwwwwwwwwwwwwwww"
"wwwwwwwwDDDwwwwwwwwwwwww"
"wwwwwDDDDDDDDDwwwwwwwwww"
"wwwDDDDDDDDDDDDDDDwwwwww"
"wwDDDDDDDDDDDwwwDDwwwwww"
"wwDDDDDDDDDDDDDDDDDwwwww"
"wwwDDDDDDDDDDDDDDDDwwwww"
"wwwwDDwDDDDDwwwDDwwwwwww"
"wwwwwwDDDDDDDDDwwwwwwwww"
"wwwwwwwwwwwwwwwwwwwwwwww"
;

Sandbox2D::Sandbox2D():Layer("Sandbox2D"),m_CameraController(1280.f/720.f, true)
{	
}

void Sandbox2D::OnAttach()
{
	//Texture Init here
	m_Texture = TGE::Texture2D::Create("assets/textures/wood.png");
	m_sheetTexture = TGE::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
	m_StairTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 7,6 }, { 128.f , 128.f }, { 1 , 1 });
	m_BarrelTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 8,2 }, { 128.f , 128.f }, { 1 , 1 });
	m_TreeTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 2,1 }, { 128.f , 128.f }, { 1 , 2 });
	m_GrassTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 1,11 }, { 128.f , 128.f }, { 1 , 1 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
	s_TextureMap['D'] = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 6,11 }, { 128.f , 128.f }, { 1 , 1 });
	s_TextureMap['w'] = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 11,11 }, { 128.f , 128.f }, { 1 , 1 });
	//Particle Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.1f, m_Particle.SizeVariation = 0.05f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);

	TGE::FrameBufferSpecification fbSpec;
	fbSpec.Width = TGE::Application::Get().GetWindow().GetWidth();//TGE::Application::Get().GetWindow().GetWidth();
	fbSpec.Height = TGE::Application::Get().GetWindow().GetHeight();//TGE::Application::Get().GetWindow().GetHeight();
	m_FrameBuffer = TGE::FrameBuffer::Create(fbSpec);
}

void Sandbox2D::OnImGuiRender()
{
	static bool dockingEnable = false;
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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

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
		//----------------Settings包含在dock的Begin和End之间，使之能够dock
		ImGui::Begin("Settings");

		auto stats = TGE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads : %d", stats.QuadCount);
		ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices : %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
		uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment();
		ImGui::Image((void*)m_TextureID, ImVec2{ 1280.f, 720.f });//将图像传入到setting页面

		ImGui::End();
		//----------------Settings

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

		ImGui::End();
	}
}

void Sandbox2D::OnUpdate(TGE::TimeStep& ts)
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
	m_CameraController.OnUpdate(ts);

	TGE::Renderer2D::ResetStats();
	TGE::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
	TGE::RenderCommand::Clear();

	//Renderer的submit函数调用RenderCommand::DrawIndex与VAO的Bind，
	//RenderCommand指定了RendererAPI成员，并调用对应API的DrawIndex
	//TGE::Renderer::BeginScene(m_CameraController.GetCamera());
#if 1
	TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());

	static float rotation = 0.0f;
	rotation += 0.002 / ts.GetTimeSeconds();

	TGE::Renderer2D::DrawQuad({ -0.5, -0.5 }, { 0.8, 0.8 }, SquareColor);
	TGE::Renderer2D::DrawRotationQuad({ 0.5, -0.5, 0.f }, { 0.75, 0.75 }, glm::vec4(0.8, 0.2, 0.3, 1.0f),-rotation);
	TGE::Renderer2D::DrawQuad({ 0.0, 0.0, -0.1 }, { 10.0, 10.0 }, m_Texture, 10.f);
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

	//Particle
	if (TGE::Input::IsMosueButtonPressed(TGE_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = TGE::Input::GetMousePosition();
		auto width = TGE::Application::Get().GetWindow().GetWidth();
		auto height = TGE::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();//相机边界
		auto pos = m_CameraController.GetCamera().GetPosition();//相机位置
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());//会开辟一个场景，并渲染粒子系统

	TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());

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
	TGE::Renderer2D::EndScene();
}


void Sandbox2D::OnEvent(TGE::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnDetach()
{
}
