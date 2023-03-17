#include "tgpch.h"
#include "Sandbox2D.h"
#include "imgui/imgui.h"
//#include "Platform/Opengl/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "TGE/Renderer/Renderer2D.h"
#include "TGE/Renderer/Texture.h"

Sandbox2D::Sandbox2D():Layer("Sandbox2D"),m_CameraController(1280.f/720.f, true)
{	
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
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

	TGE::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
	TGE::RenderCommand::Clear();

	//Renderer的submit函数调用RenderCommand::DrawIndex与VAO的Bind，
	//RenderCommand指定了RendererAPI成员，并调用对应API的DrawIndex
	//TGE::Renderer::BeginScene(m_CameraController.GetCamera());
	TGE::Renderer2D::BeginScene(m_CameraController.GetCamera());

	//m_Shader->Bind();
	//std::dynamic_pointer_cast<TGE::OpenGLShader>(m_Shader)->SetUniformFloat4("uColor", SquareColor);
	//TGE::Renderer::Submit(m_Shader, m_VertexArray);
	TGE::Renderer2D::DrawQuad({ 0.0, 0.0 }, { 1.0, 1.0 }, SquareColor);
	TGE::Renderer2D::DrawQuad({ -1.0, 0.0 }, { 0.5, 0.8 }, m_Texture);

	//TGE::Renderer::EndScene();
	TGE::Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(TGE::Event& event)
{
	m_CameraController.OnEvent(event);
}

void Sandbox2D::OnAttach()
{
	m_Texture = TGE::Texture2D::Create("assets/textures/wood.png");
}

void Sandbox2D::OnDetach()
{
}
