#include "tgpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
//#include "Platform/Opengl/ImGuiOpenGL.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "TGE/Application.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace TGE {
	ImGuiLayer::ImGuiLayer():Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		//ImGui::CreateContext();
		//ImGui::StyleColorsDark();

		//ImGuiIO& io = ImGui::GetIO();
		////或运算取大 与运算取小 异或运算减法
		//io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		//io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  //      io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		//io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		//io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		//io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		//io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		//io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		//io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		//io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		//io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		//io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		//io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		//io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		//io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		//io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		//io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		//io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		//io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		//io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		//io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		//io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		//ImGui_ImplOpenGL3_Init("#version 410");
		//-----------------------------------------------------
		//from C:\Engine\TGE\vendor\imgui\examples\example_glfw_opengl3\main.cpp
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		//Setup Platform/Renderer bindings
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
		//ImGui_ImplOpenGL3_Init("version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	//void ImGuiLayer::OnUpdate() 
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	Application& app = Application::Get();
	//	io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

	//	float time = (float)glfwGetTime();
	//	io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);
	//	m_Time = time;

	//	ImGui_ImplOpenGL3_NewFrame();
	//	ImGui::NewFrame();

	//	static bool show = true;
	//	ImGui::ShowDemoWindow(&show);

	//	ImGui::Render();
	//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//}

	//void ImGuiLayer::OnEvent(Event& event)
	//{
	//	EventDispatcher dispatcher(event);
	//	dispatcher.Dispatch<MouseButtonPressedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
	//	dispatcher.Dispatch<MouseButtonReleasedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
	//	dispatcher.Dispatch<MouseScrolledEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
	//	dispatcher.Dispatch<MouseMovedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
	//	dispatcher.Dispatch<KeyPressedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
	//	dispatcher.Dispatch<KeyReleasedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
	//	dispatcher.Dispatch<KeyTypedEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
	//	dispatcher.Dispatch<WindowResizeEvent>(TGE_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));

	//}
	//bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseDown[e.GetMouseButton()] = true;
	//	return false;
	//}
	//bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseDown[e.GetMouseButton()] = false;
	//	return false;
	//}
	//bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseWheelH += e.GetXOffset();
	//	io.MouseWheel += e.GetYOffset();
	//	return false;
	//}
	//bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.KeysDown[e.GetKeyCode()] = true;

	//	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	//	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	//	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	//	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	//	return false;
	//}
	//bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.KeysDown[e.GetKeyCode()] = false;
	//	return false;
	//}
	//bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	int keycode = e.GetKeyCode();
	//	if (keycode > 0 && keycode < 0x10000)
	//		io.AddInputCharacter((unsigned short)keycode);
	//	return false;
	//}
	////bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	////{
	////	return false;
	////}
	//bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MousePos = ImVec2(e.GetX(), e.GetY());
	//	return false;
	//}
	//bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
	//	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	//	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	//	return false;
	//}
}