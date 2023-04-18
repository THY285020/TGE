#include "tgpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace TGE {
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//混合因子，混合方法

		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}
	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexcount)
	{
		uint32_t count = indexcount ? vertexArray->GetIndexBuffer()->GetCount():indexcount;
		//(mode, count, type, offset)
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);//不在使用索引缓冲对象的时候填入数组
		glBindTexture(GL_TEXTURE_2D, 0);//解除绑定
	}
}

