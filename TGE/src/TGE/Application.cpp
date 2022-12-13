#include "tgpch.h"
#include "Application.h"
#include "TGE/Window.h"
//#include "TGE.h"

#include <glad/glad.h>
#include "Input.h"

#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include "TGE/Renderer/Renderer.h"

//Application->创建window->window调用glfw->glfw反馈信息(event)传递给Window的eventcallback，而SetEventCallback将eventcallback设置为APP::OnEvent，
// 也就是把相应的event传递给OnEvent->OnEvent调用dispatch检验类型，调用相应函数
//Application->pushOverLay->ImGuiLayer::OnAttach->(ImGuiLayer::OnEvent)=》Application Run()->ImGuiLayer::OnImGuiRender
//Application->Run->Input::GetMousePosition->WindowInput::GetMousePositionImpl->glfw
//App->pushLayer->Layer->attach

namespace TGE {
	//bind 第一个参数是函数指针 第二个是成员函数的对象 然后是参数：placeholders_1表示占位符，参数以调用时传入为准 
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TGE_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//SetEventCallback函数让m_Window的m_Data结构体的callback函数变为(fn)
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
		//Dispatch函数检验e的类型是否是WindowCloseEvent 是则应用后面函数
		dispatcher.Dispatch<WindowCloseEvent>(TGE_BIND_EVENT_FN(Application::OnWindowClose));
		//TGE_CORE_TRACE("{0}", e);//Sandbox处使用 

		//从后往前处理
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
		//	TGE_TRACE(e);//Event.h中添加了输出流运算符
		//}
		//if (e.IsInCategory(EventCategoryInput)) {
		//	TGE_TRACE(e);//e不属于Input类型
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

			//Renderer的submit函数调用RenderCommand::DrawIndex与VAO的Bind，
			//RenderCommand指定RendererAPI成员，调用对应API的DrawIndex
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
				//OnImGuiRender方法用于渲染ImGui窗口，可自定义Layer，重载OnImGuiRender创建不同窗口
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
