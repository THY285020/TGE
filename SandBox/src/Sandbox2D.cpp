#include "tgpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
//#include "Platform/Opengl/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D():Layer("Sandbox2D"),m_CameraController(1280.f/720.f, true)
{	
}

void Sandbox2D::OnImGuiRender()
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
#if 0
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
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());

	//SubTextureScene
	TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());
	TGE::Renderer2D::DrawQuad({ 0.0, 0.0, 0.0 }, { 1.0, 1.0 }, m_StairTexture, 1.f);
	TGE::Renderer2D::DrawQuad({ 1.0, 0.0, 0.0 }, { 1.0, 1.0 }, m_BarrelTexture, 1.f);
	TGE::Renderer2D::DrawQuad({ -1.5, 0.0, 0.0 }, { 1.0, 1.0 }, m_TreeTexture, 1.f);
	TGE::Renderer2D::EndScene();
}


void Sandbox2D::OnEvent(TGE::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnAttach()
{
	m_Texture = TGE::Texture2D::Create("assets/textures/wood.png");
	m_sheetTexture = TGE::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
	m_StairTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 7,6 }, { 128.f , 128.f }, { 1 , 1 });
	m_BarrelTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 8,2 }, { 128.f , 128.f }, { 1 , 1 });
	m_TreeTexture = TGE::SubTexture2D::CreateFromCoords(m_sheetTexture, { 2,1 }, { 128.f , 128.f }, { 1 , 2});
	//Particle Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.1f, m_Particle.SizeVariation = 0.05f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
}
