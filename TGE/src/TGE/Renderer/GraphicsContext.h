#pragma once

//WindowsWindow-¡·GraphicsContext-¡·OpenglContext-¡·glfwCreateWindow->makeContextCurrent¡¢SwapBuffers
namespace TGE {
	class GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}