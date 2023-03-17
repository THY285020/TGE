#include "tgpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include"TGE/Renderer/RenderCommand.h"
#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		//Ref<Shader> FlatColorShader;
		Ref<Shader> TextureShader;
		Ref<Texture> WhiteTexture;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create();
		float squareVertices[4 * 5] = {
			//Position		   //TexCoord/
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,//左下
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,//右下
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,//右上
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f //左上
		};					   
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float2, "TexCoord" }
			});

		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		//为了透明纹理，需要延后输入纹理数据
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t WhiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&WhiteTextureData, sizeof(uint32_t));
		//s_Data->FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");
		s_Data->TextureShader = Shader::Create("assets/shaders/Texture_s.glsl");
		
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("Texture", 0);
	}

	void Renderer2D::shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		//s_Data->FlatColorShader->Bind();
		//s_Data->FlatColorShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		/*s_Data->TextureShader->Bind();*/
		s_Data->TextureShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->WhiteTexture->Bind(0);
		s_Data->TextureShader->SetFloat4("Color", color);

		glm::mat4 transform = glm::translate(glm::mat4(1.0), pos) * 
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("Transfrom", transform);
	
		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndex(s_Data->QuadVertexArray);
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture)
	{
		s_Data->TextureShader->SetFloat4("Color", {1.f, 1.f, 1.f, 1.f});
		texture->Bind(0);
		glm::mat4 transform = glm::translate(glm::mat4(1.0), pos) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->TextureShader->SetMat4("Transfrom", transform);

		s_Data->QuadVertexArray->Bind();
		RenderCommand::DrawIndex(s_Data->QuadVertexArray);
		
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, texture);
	}
}

