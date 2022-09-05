#pragma once
#include "core.h"
#include "Events/Event.h"
#include "TGE/Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "TGE/ImGui/ImGuiLayer.h"
#include "Window.h"

#include "TGE/Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Buffer.h"


#include "TGE/Renderer/OrthoCamera.h"
//glfw提取输入信息-》dispatcher根据event类型绑定函数-》OnEvent处理(目前打印)信息
namespace TGE {
	class TGE_API Application
	{
	public:
		Application();//创建窗口，调用SetEventCallback，设置反馈绑定OnEvent
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		bool m_Running = true;
		static Application* s_Instance;

		//Render
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		//std::shared_ptr<VertexBuffer> m_VertexBuffer;
		//std::shared_ptr<IndexBuffer> m_IndexBuffer;
		
		std::shared_ptr<Shader> m_Shader2;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthoCamera m_Camera;
	};
	//To be defined in Client
	Application* CreateApplication();
}


