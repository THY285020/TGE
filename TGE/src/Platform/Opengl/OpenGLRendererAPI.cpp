#include "tgpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace TGE {
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//混合因子，混合方法

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_LINE_SMOOTH);//传统AA
		glEnable(GL_MULTISAMPLE);//MSAA

		glEnable(GL_STENCIL_TEST);//模板测试
	}
	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::SetCullFaceFront()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	void OpenGLRendererAPI::SetCullFaceBack()
	{
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::CloseCullFace()
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexcount)
	{
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, nullptr);//不在使用索引缓冲对象的时候填入数组
	}

	void OpenGLRendererAPI::DrawIndexTS(const Ref<VertexArray>& vertexArray, uint32_t indexcount)
	{
		//glPolygonMode(GL_FRONT, GL_LINE);//线框模式 GL_POINT GL_FILL
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLE_STRIP, indexcount, GL_UNSIGNED_INT, 0);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexcount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexcount);
	}

	void OpenGLRendererAPI::DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexcount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexcount);
	}

	void OpenGLRendererAPI::DrawArraysTS(const Ref<VertexArray>& vertexArray, uint32_t vertexcount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexcount);
	}

	void OpenGLRendererAPI::DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t vertexcount)
	{
		vertexArray->Bind();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, vertexcount, GL_UNSIGNED_INT, 0);
		
		//glDrawArrays(GL_PATCHES, 0, vertexcount);
	}

	void OpenGLRendererAPI::SetLineWidth(float thickness)
	{
		glLineWidth(thickness);
	}

}

