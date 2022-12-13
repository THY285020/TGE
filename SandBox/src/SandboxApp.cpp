#include "tgpch.h"
#include "TGE.h"
#include "Platform/Opengl/OpenGLShader.h"
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4


#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
//glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
//{
//	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
//	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
//	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
//	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
//	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
//	return Projection * View * Model;
//}

class ExampleLayer : public TGE::Layer {
public:
	ExampleLayer():Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_CameraPos = m_Camera.GetPosition();
		m_CameraRot = m_Camera.GetRotation();
		m_VertexArray.reset(TGE::VertexArray::Create());
		//glGenVertexArrays glBindVertexArray

		float vertices[3 * 7] = {
			//position			//color
			-0.5f, -0.5f, 0.0f, 1.0, 0.0, 1.0, 1.0,
			 0.5f, -0.5f, 0.0f, 0.0, 1.0, 0.0, 1.0,
			 0.0f,  0.5f, 0.0f, 0.0, 0.0, 1.0, 1.0
		};

		TGE::Ref<TGE::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(TGE::VertexBuffer::Create(vertices, sizeof(vertices)));
		////�˴�reset���ͷ�����ָ��ԭ���Ŀռ�
		//glGenBuffers glBindBuffer glBufferData

		TGE::BufferLayout layout = {
			{ TGE::ShaderDataType::Float3, "Position" },
			{ TGE::ShaderDataType::Float4, "Color" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };

		TGE::Ref<TGE::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(TGE::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			layout (location = 0) in vec3 Position;
			layout (location = 1) in vec4 Color;
			
			uniform mat4 ViewProj;
			uniform mat4 Transfrom;
			
			out vec3 fragPos;
			out vec4 color;

			void main(){
				color = Color;
				fragPos = Position;
				gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
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

		m_Shader.reset(TGE::Shader::Create(vertexSrc, fragmentSrc));

		//shader2--------------------------------------------

		m_SquareVA.reset(TGE::VertexArray::Create());
		float squareVertices[4 * 5] = {
			//Position			//TexCoord
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//����
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,//����
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//����
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f //����
		};
		TGE::Ref<TGE::VertexBuffer> squareVB;
		squareVB.reset(TGE::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ TGE::ShaderDataType::Float3, "Position" },
			{ TGE::ShaderDataType::Float2, "TexCoord" }
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		TGE::Ref<TGE::IndexBuffer> squareIB;
		squareIB.reset(TGE::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc2 = R"(
			#version 330 core
			layout (location = 0) in vec3 Position;
			
			uniform mat4 ViewProj;
			uniform mat4 Transfrom;
			out vec3 fragPos;

			void main(){
				fragPos = Position;
				gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core
			in vec3 fragPos;
			uniform vec3 Color;

			out vec4 FragColor;

			void main(){
				//FragColor = vec4(fragPos*0.5 + 0.5, 1.0);
				FragColor = vec4(Color, 1.0);
			}
		)";

		m_SquareShader.reset(TGE::Shader::Create(vertexSrc2, fragmentSrc2));
		//--------------------------TextureShader
		//std::string vertexSrcT = R"(
		//	#version 330 core
		//	layout (location = 0) in vec3 Position;
		//	layout (location = 1) in vec2 TexCoord;

		//	uniform mat4 ViewProj;
		//	uniform mat4 Transfrom;
		//	out vec2 f_TexCoord;

		//	void main(){
		//		f_TexCoord = TexCoord;
		//		gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
		//	}
		//)";

		//std::string fragmentSrcT = R"(
		//	#version 330 core
		//	in vec2 f_TexCoord;

		//	uniform sampler2D Texture;

		//	out vec4 FragColor;

		//	void main(){
		//		//FragColor = vec4(f_TexCoord, 0.0f, 1.0f);
		//		FragColor = texture(Texture, f_TexCoord);
		//	}
		//)";

		m_TextureShader.reset(TGE::Shader::Create("assets/shaders/Texture.glsl"));

		m_Texture = (TGE::Texture2D::Create("assets/textures/wood.png"));//�˴��Ѿ����ع���ָ�룬��������reset

		std::dynamic_pointer_cast<TGE::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<TGE::OpenGLShader>(m_TextureShader)->SetUniformInt("Texture", 0);
	}

	virtual void OnImGuiRender() override {
		//ImGui::Begin("Test");
		//ImGui::Text("Hello World!");
		//ImGui::End();
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(SquareColor));
		ImGui::End();
	}

	void OnUpdate(TGE::TimeStep& ts) override
	{	
		//--------interval--------------
		if (time_temp >= 1.0) {
			TGE_TRACE("fps: {0} ({1}ms)", int(1 / ts.GetTimeSeconds()), ts.GetTimemillSeconds());
			time_temp = 0.0;
		}
		else {
			time_temp += ts.GetTimeSeconds();
		}
		//--------Tab------------
		if (TGE::Input::IsKeyPressed(TGE_KEY_TAB)) {
			TGE_INFO("Tab key is pressed!(poll)");
		}
		//-------------CameraMove----------------
		if (TGE::Input::IsKeyPressed(TGE_KEY_W) * ts)
		{
			m_CameraPos.z -= m_CameraSpeed * ts;
		}
		else if (TGE::Input::IsKeyPressed(TGE_KEY_S) * ts)
		{
			m_CameraPos.z += m_CameraSpeed * ts;
		}

		if (TGE::Input::IsKeyPressed(TGE_KEY_A) * ts)
		{
			m_CameraPos.x -= m_CameraSpeed * ts;
		}
		else if (TGE::Input::IsKeyPressed(TGE_KEY_D) * ts)
		{
			m_CameraPos.x += m_CameraSpeed * ts;
		}

		if (TGE::Input::IsKeyPressed(TGE_KEY_Q) * ts)
		{
			m_CameraPos.y += m_CameraSpeed * ts;
		}
		else if (TGE::Input::IsKeyPressed(TGE_KEY_E) )
		{
			m_CameraPos.y -= m_CameraSpeed * ts;
		}
		//------------Rotation--------------------
		if (TGE::Input::IsKeyPressed(TGE_KEY_R) )
		{
			m_CameraRot += m_CameraRotSpeed * ts;
		}
		//------------transfromation----------------
		if (TGE::Input::IsKeyPressed(TGE_KEY_LEFT) * ts)
		{
			m_SquareVA_Pos.x -= m_CameraSpeed * ts;
		}
		else if (TGE::Input::IsKeyPressed(TGE_KEY_RIGHT) * ts)
		{
			m_SquareVA_Pos.x += m_CameraSpeed * ts;
		}

		if (TGE::Input::IsKeyPressed(TGE_KEY_UP) * ts)
		{
			m_SquareVA_Pos.y += m_CameraSpeed * ts;
		}
		else if (TGE::Input::IsKeyPressed(TGE_KEY_DOWN))
		{
			m_SquareVA_Pos.y -= m_CameraSpeed * ts;
		}

		/*auto cam = camera(5.0f, { 0.5f, 0.5f });*/
		//RenderCommand->RenderAPI->OpenGLRendererAPI
		TGE::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		TGE::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPos);
		m_Camera.SetRotation(m_CameraRot);

		//Renderer��submit��������RenderCommand::DrawIndex��VAO��Bind��
		//RenderCommandָ����RendererAPI��Ա�������ö�ӦAPI��DrawIndex
		TGE::Renderer::BeginScene(m_Camera);

		//m_SquareShader->Bind();
		//m_SquareShader->SetUniformMat4("ViewProj",m_Camera.GetViewProjectionMatrix());
		m_SquareVA_Scale = glm::vec3(0.1f);
		
		//glm::vec4 redColor(0.8, 0.2, 0.3, 1.0);
		//glm::vec4 blueColor(0.2, 0.3, 0.8, 1.0);
		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				glm::vec3 m_Pos = glm::vec3(0.11 * i, 0.11 * j, 0) + m_SquareVA_Pos;
				TGE::Renderer::Submit(m_SquareShader, m_SquareVA, m_Pos, m_SquareVA_Scale);
				std::dynamic_pointer_cast<TGE::OpenGLShader>(m_SquareShader)->SetUniformFloat3("Color", SquareColor);
			}
		}

		m_Texture->Bind(0);
		TGE::Renderer::Submit(m_TextureShader, m_SquareVA, glm::vec3(0.0), glm::vec3(1.0f));

		//Triangle
		/*TGE::Renderer::Submit(m_Shader, m_VertexArray);*/

		TGE::Renderer::EndScene();
	}

	void OnEvent(TGE::Event& event) override
	{
		//TGE_TRACE("{0}", event);
		//if (event.GetEventType() == TGE::EventType::KeyPressed) {
		//	TGE::KeyPressedEvent& e = (TGE::KeyPressedEvent&)event;
		//	TGE_TRACE("{0}", (char)e.GetKeyCode());
		//}

		/*TGE::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TGE::KeyPressedEvent>(TGE_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));*/
	}
private:
	//Render
	TGE::Ref<TGE::Shader> m_Shader;
	TGE::Ref<TGE::VertexArray> m_VertexArray;
	//std::shared_ptr<VertexBuffer> m_VertexBuffer;
	//std::shared_ptr<IndexBuffer> m_IndexBuffer;

	TGE::Ref<TGE::Shader> m_SquareShader;
	TGE::Ref<TGE::VertexArray> m_SquareVA;

	TGE::Ref<TGE::Shader> m_TextureShader;
	TGE::Ref<TGE::Texture2D> m_Texture;

	TGE::OrthoCamera m_Camera;
	glm::vec3 m_CameraPos;
	float m_CameraRot;
	float m_CameraSpeed = 10.f;
	float m_CameraRotSpeed = 180.0f;

	double time_temp = 0.0;
	glm::vec3 m_SquareVA_Pos = glm::vec3(0.0);
	glm::vec3 m_SquareVA_Scale = glm::vec3(1.0);

	glm::vec3 SquareColor = glm::vec3(0.2, 0.3, 0.8);

};

class Sandbox :public TGE::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		/*PushOverlay(new TGE::ImGuiLayer());*/
	}
	~Sandbox() {

	}
};

TGE::Application* TGE::CreateApplication() {
	return new Sandbox();
}