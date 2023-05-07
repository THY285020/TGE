#include "tgpch.h"
#include "WindowsWindow.h"
#include "TGE/Core/log.h"

#include "TGE/Events/KeyEvent.h"
#include "TGE/Events/MouseEvent.h"
#include "TGE/Events/ApplicationEvent.h"


#include "Platform/Opengl/OpenGLContext.h"

#include <glad/glad.h>


namespace TGE {
	//������Ҫ���window���Ծ�̬
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		TGE_CORE_ERROR("GLFW ERROR ({0}); {1}", error, description);
	}
	//���ù��캯��
	Window* Window::Create(const WindowProps& props) {
		return new WindowsWindow(props);
	}
	//���캯������Init
	WindowsWindow::WindowsWindow(const WindowProps& props) {
		Init(props);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}
	//Init��ʼ��glfw ����window��������
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		TGE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Height, props.Width);


		if (!s_GLFWInitialized) {
			int success = glfwInit();
			/*TGE_CORE_WARN(success, "Could not initialize GLFW!");*/
			TGE_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = new OpenGLContext(m_Window);//��֧�ֶ���API�ɱ�дD3DContex��VulkanContext��
		m_Context->Init();
		//glfwMakeContextCurrent(m_Window);
		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//TGE_CORE_ASSERT(status, "Failed to initialize Glad!")

		glfwSetWindowUserPointer(m_Window, &m_Data);//�����û�ָ��Ϊm_Data
		SetVSync(true);

		//Set GLFW callbacks ע��ص�������glfw������Ϣ���û����庯��
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));//�����û�ָ���WindowData�ṹ��
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			//Applicaiton����ʱ����SetEventCallback��EventCallback����ΪOnEvent����OnEvent������Ϣ
			data.EventCallback(event);//�Ѵ��ڱ仯event���뵽event������
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) 
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
			
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}	
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}	
			}
		});
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) 
		{
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			MouseMovedEvent event((float)xpos, (float)ypos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
		//glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}


	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
}



