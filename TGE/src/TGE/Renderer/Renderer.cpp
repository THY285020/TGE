#include "tgpch.h"
#include "Renderer.h"

#include "Platform/Opengl/OpenGLShader.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

namespace TGE {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}
	void TGE::Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
	void Renderer::BeginScene(OrthoCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
	{
		shader->Bind();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("ViewProj", m_SceneData->ViewProjectionMatrix);
		glm::mat4 transfrom = glm::mat4(1.0);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("Transfrom", transfrom);

		vertexArray->Bind();
		RenderCommand::DrawIndex(vertexArray);

	}
	//ÓÐposition
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::vec3& pos)
	{
		shader->Bind();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("ViewProj", m_SceneData->ViewProjectionMatrix);
		glm::mat4 transfrom = glm::translate(glm::mat4(1.0), pos);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("Transfrom", transfrom);

		vertexArray->Bind();
		RenderCommand::DrawIndex(vertexArray);
	}
	//ÓÐposition£¬ÓÐscale
	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::vec3& pos, const glm::vec3& scal)
	{
		shader->Bind();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("ViewProj", m_SceneData->ViewProjectionMatrix);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), scal);
		glm::mat4 transfrom = glm::translate(glm::mat4(1.0), pos)*scale;
		std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("Transfrom", transfrom);

		vertexArray->Bind();
		RenderCommand::DrawIndex(vertexArray);
	}
	
}