#include "tgpch.h"
#include "Application.h"
#include "TGE/Window.h"
//#include "TGE.h"

#include <glad/glad.h>
#include "Input.h"

#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include "TGE/Renderer/Renderer.h"

//Application->����window->window����glfw->glfw������Ϣ(event)���ݸ�Window��eventcallback����SetEventCallback��eventcallback����ΪAPP::OnEvent��
// Ҳ���ǰ���Ӧ��event���ݸ�OnEvent->OnEvent����dispatch�������ͣ�������Ӧ����
//Application->pushOverLay->ImGuiLayer::OnAttach->(ImGuiLayer::OnEvent)=��Application Run()->ImGuiLayer::OnImGuiRender
//Application->Run->Input::GetMousePosition->WindowInput::GetMousePositionImpl->glfw
//App->pushLayer->Layer->attach

namespace TGE {
	//bind ��һ�������Ǻ���ָ�� �ڶ����ǳ�Ա�����Ķ��� Ȼ���ǲ�����placeholders_1��ʾռλ���������Ե���ʱ����Ϊ׼ 
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TGE_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//SetEventCallback������m_Window��m_Data�ṹ���callback������Ϊ(fn)
		m_Window->SetEventCallback(TGE_BIND_EVENT_FN(Application::OnEvent));

		RenderCommand::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);


	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//Dispatch��������e�������Ƿ���WindowCloseEvent ����Ӧ�ú��溯��
		dispatcher.Dispatch<WindowCloseEvent>(TGE_BIND_EVENT_FN(Application::OnWindowClose));
		//TGE_CORE_TRACE("{0}", e);//Sandbox��ʹ�� 

		//�Ӻ���ǰ����
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run() {
		//WindowResizeEvent e(1280, 720);
		//if (e.IsInCategory(EventCategoryApplication)) {
		//	TGE_TRACE(e);//Event.h�����������������
		//}
		//if (e.IsInCategory(EventCategoryInput)) {
		//	TGE_TRACE(e);//e������Input����
		//}
		TimeStep timestep(0.0);
		while (m_Running) {
			float time = (float)glfwGetTime();//Platform::GetTime
			timestep = (time - m_LastFrameTime);
			m_LastFrameTime = time;

			//glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			
			/*RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
			RenderCommand::Clear();*/

			//m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
			//m_Camera.SetRotation(45.0f);

			//Renderer��submit��������RenderCommand::DrawIndex��VAO��Bind��
			//RenderCommandָ��RendererAPI��Ա�����ö�ӦAPI��DrawIndex
			/*Renderer::BeginScene(m_Camera);*/

			//m_Shader2->Bind();
			//m_Shader2->SetUniformMat4("ViewProj",m_Camera.GetViewProjectionMatrix());
			/*Renderer::Submit(m_Shader2, m_SquareVA);*/

			//m_Shader->Bind();
			//m_Shader->SetUniformMat4("ViewProj", m_Camera.GetViewProjectionMatrix());
			/*Renderer::Submit(m_Shader, m_VertexArray);

			Renderer::EndScene();*/

			/*m_Shader2->Bind();
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);*/

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				//OnImGuiRender����������ȾImGui���ڣ����Զ���Layer������OnImGuiRender������ͬ����
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			//auto [x, y] = Input::GetMousePosition();
			//TGE_CORE_TRACE("{0}, {1}", x, y);
			m_Window->OnUpdate();
			//=glfwPollEvents();
			//+glfwSwapBuffers(m_Window)
		};
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
