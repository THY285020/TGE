#include "tgpch.h"
#include "Application.h"
#include "TGE/Window.h"

#include <glad/glad.h>
#include "Input.h"

#include <glm/glm.hpp>
#include "TGE/Renderer/Renderer.h"

//Application->创建window->window调用glfw->glfw反馈信息给OnEvent->OnEvent调用dispatch检验类型，调用相应函数
//Application->pushOverLay->ImGuiLayer::OnAttach->(ImGuiLayer::OnEvent)=》Application Run()->ImGuiLayer::OnImGuiRender
//Application->Run->Input::GetMousePosition->WindowInput::GetMousePositionImpl->glfw
//App->pushLayer->Layer->attach

namespace TGE {
	//bind 第一个参数是函数指针 第二个是成员函数的对象 然后是参数：placeholders_1表示占位符，参数以调用时传入为准 
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		TGE_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//SetEventCallback函数让m_Window的m_Data结构体的callback函数变为(fn)
		m_Window->SetEventCallback(TGE_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		//OpenGL

		m_VertexArray.reset(VertexArray::Create());
		//glGenVertexArrays(1, &m_VertexArray);
		//glBindVertexArray(m_VertexArray);

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0, 0.0, 1.0, 1.0,
			 0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0, 1.0,
			 0.0f,  0.5f, 0.0f, 0.0, 0.0, 1.0, 1.0 
		};

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));//glGenBuffers glBindBuffer glBufferData

		BufferLayout layout = {
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		//const auto& layout = m_VertexBuffer->GetLayout();
		//uint32_t index = 0;
		//for (const auto& element : layout) 
		//{
		//	glEnableVertexAttribArray(index);
		//	//GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer
		//	glVertexAttribPointer(index, 
		//		element.GetComponentCount(), 
		//		ShaderDataTypeToOpenGLBaseType(element.Type), 
		//		element.Normalized ? GL_TRUE : GL_FALSE, 
		//		layout.GetStride(), 
		//		(const void*)element.Offset);
		//	index++;
		//}

		uint32_t indices[3] = { 0, 1, 2 };

		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			layout (location = 0) in vec3 Position;
			layout (location = 1) in vec4 Color;
			
			uniform mat4 ViewProj;
			
			out vec3 fragPos;
			out vec4 color;

			void main(){
				color = Color;
				fragPos = Position;
				gl_Position = ViewProj * vec4(Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			in vec4 color;
			in vec3 fragPos;
			out vec4 FragColor;

			void main(){
				//FragColor = vec4(fragPos*0.5 + 0.5, 1.0);
				FragColor = color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		//shader2--------------------------------------------

		m_SquareVA.reset(VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "Position" }
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0};

		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc2 = R"(
			#version 330 core
			layout (location = 0) in vec3 Position;
			
			uniform mat4 ViewProj;
			out vec3 fragPos;

			void main(){
				fragPos = Position;
				gl_Position = ViewProj * vec4(Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core
			in vec3 fragPos;
			out vec4 FragColor;

			void main(){
				FragColor = vec4(fragPos*0.5 + 0.5, 1.0);
			}
		)";

		m_Shader2.reset(new Shader(vertexSrc2, fragmentSrc2));

	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//检验e的类型是否是WindowCloseEvent 是则应用后面函数
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

		while (m_Running) {
			//glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			
			RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
			RenderCommand::Clear();

			m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
			m_Camera.SetRotation(45.0f);

			//Renderer的submit函数调用RenderCommand::DrawIndex与VAO的Bind，
			//RenderCommand指定RendererAPI成员，调用对应API的DrawIndex
			Renderer::BeginScene(m_Camera);

			//m_Shader2->Bind();
			//m_Shader2->SetUniformMat4("ViewProj",m_Camera.GetViewProjectionMatrix());
			Renderer::Submit(m_Shader2, m_SquareVA);

			//m_Shader->Bind();
			//m_Shader->SetUniformMat4("ViewProj", m_Camera.GetViewProjectionMatrix());
			Renderer::Submit(m_Shader, m_VertexArray);

			Renderer::EndScene();

			/*m_Shader2->Bind();
			m_SquareVA->Bind();
			glDrawElements(GL_TRIANGLES, m_SquareVA->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);*/

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

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
