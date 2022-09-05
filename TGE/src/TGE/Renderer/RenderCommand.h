#pragma once

#include "RendererAPI.h"
//Renderer::Submit->RenderCommand::DrawIndex->RendererAPI=OpenGLRenderAPI->OpenGLRenderAPI::SetClearColor/Clear/DrawIndex
namespace TGE {
	class RenderCommand 
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) 
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void DrawIndex(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndex(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}