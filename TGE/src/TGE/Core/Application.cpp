#include "tgpch.h"
#include "Application.h"
#include "TGE/Core/Window.h"
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

	Application::Application(const std::string& name)
	{
		TGE_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		//SetEventCallback������m_Window��m_Data�ṹ���callback������Ϊ(fn)
		m_Window->SetEventCallback(TGE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);//overlay������Ⱦ���ڻ��涥��

	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//Dispatch��������e�������Ƿ���WindowCloseEvent ����Ӧ�ú��溯��
		dispatcher.Dispatch<WindowCloseEvent>(TGE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(TGE_BIND_EVENT_FN(Application::OnWindowResizeEvent));
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
		TimeStep timestep(0.0);
		while (m_Running) {
			float time = (float)glfwGetTime();//Platform::GetTime
			timestep = (time - m_LastFrameTime);
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				//OnImGuiRender����������ȾImGui���ڣ����Զ���Layer������OnImGuiRender������ͬ����
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		};
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		if (e.GetHeight() == 0 || e.GetWidth() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetHeight(), e.GetWidth());
		return true;
	}
}
