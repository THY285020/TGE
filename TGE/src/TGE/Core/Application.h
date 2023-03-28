#pragma once
#include "core.h"
#include "../Events/Event.h"
#include "TGE/Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "TGE/ImGui/ImGuiLayer.h"
#include "Window.h"
#include "TimeStep.h"

//#include "TGE/Renderer/Shader.h"
//#include "Renderer/VertexArray.h"
//#include "Renderer/Buffer.h"
//
//
//#include "TGE/Renderer/OrthoCamera.h"
//glfw��ȡ������Ϣ-��dispatcher����event���Ͱ󶨺���-��OnEvent����(Ŀǰ��ӡ)��Ϣ
namespace TGE {
	class TGE_API Application
	{
	public:
		Application(const std::string& name = "TGE Engine");//�������ڣ�����SetEventCallback�����÷�����OnEvent
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void Close();
	private:

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		std::unique_ptr<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		bool m_Running = true;

		float m_LastFrameTime;

		static Application* s_Instance;

		bool m_Minimized = false;
	};
	//To be defined in Client
	Application* CreateApplication();
}


