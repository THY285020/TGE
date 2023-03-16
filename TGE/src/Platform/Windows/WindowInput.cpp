#include "tgpch.h"
#include "WindowInput.h"

#include <GLFW/glfw3.h>
#include "TGE/Core/Application.h"

//Application->s_Instance->Window* m_Window->GLFWwindow* m_Window
namespace TGE { 
	Input* Input::s_Instance = new WindowInput();
//Key
	bool WindowInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
//MouseButton
	bool WindowInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
//MousePosition
	std::pair<float, float> WindowInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return {(float)xpos, (float)ypos};
	}

	float WindowInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return x;
	}
	float WindowInput::GetMouseYImpl()
	{
		//auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//double xpos, ypos;
		//glfwGetCursorPos(window, &xpos, &ypos);
		//return (float)ypos;
		//C++17ÐÂÌØÐÔ
		auto [x, y] = GetMousePositionImpl();
		return y;
	}

}