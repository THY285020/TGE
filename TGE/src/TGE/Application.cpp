#include "tgpch.h"
#include "Application.h"
#include "TGE/Window.h"

#include <glad/glad.h>
#include "Input.h"

#include <glm/glm.hpp>
#include "TGE/Renderer/Renderer.h"

//Application->����window->window����glfw->glfw������Ϣ��OnEvent->OnEvent����dispatch�������ͣ�������Ӧ����
//Application->pushOverLay->ImGuiLayer::OnAttach->(ImGuiLayer::OnEvent)=��Application Run()->ImGuiLayer::OnImGuiRender
//Application->Run->Input::GetMousePosition->WindowInput::GetMousePositionImpl->glfw
//App->pushLayer->Layer->attach

namespace TGE {
	//bind ��һ�������Ǻ���ָ�� �ڶ����ǳ�Ա�����Ķ��� Ȼ���ǲ�����placeholders_1��ʾռλ���������Ե���ʱ����Ϊ׼ 
	//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		TGE_CORE_ASSERT(!s_Instance, "Application already exists!")
			s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//SetEventCallback������m_Window��m_Data�ṹ���callback������Ϊ(fn)
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
		//����e�������Ƿ���WindowCloseEvent ����Ӧ�ú��溯��
		dispatcher.Dispatch<WindowCloseEvent>(TGE_BIND_EVENT_FN(Application::OnWindowClose));
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
		//WindowResizeEvent e(1280, 720);
		//if (e.IsInCategory(EventCategoryApplication)) {
		//	TGE_TRACE(e);//Event.h�����������������
		//}
		//if (e.IsInCategory(EventCategoryInput)) {
		//	TGE_TRACE(e);//e������Input����
		//}

		while (m_Running) {
			//glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT);
			
			RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
			RenderCommand::Clear();

			m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
			m_Camera.SetRotation(45.0f);

			//Renderer��submit��������RenderCommand::DrawIndex��VAO��Bind��
			//RenderCommandָ��RendererAPI��Ա�����ö�ӦAPI��DrawIndex
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
				//OnImGuiRender����������ȾImGui���ڣ����Զ���Layer������OnImGuiRender������ͬ����
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
