#include "tgpch.h"
#include "OpenGLContext.h"

#include<GLFW/glfw3.h>
#include <glad/glad.h>

namespace TGE {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_windowHandle(windowHandle)
	{
		TGE_CORE_ASSERT(windowHandle, "WindowHandle is null!")
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TGE_CORE_ASSERT(status, "Failed to initialize Glad!")
		
		TGE_CORE_INFO("OpenGL Info:")
		TGE_CORE_INFO(" Vendor:{0}", glGetString(GL_VENDOR))
		TGE_CORE_INFO(" Renderer:{0}", glGetString(GL_RENDERER))
		TGE_CORE_INFO(" Version:{0}", glGetString(GL_VERSION))
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}

