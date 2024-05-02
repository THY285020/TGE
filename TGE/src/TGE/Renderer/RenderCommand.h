#pragma once

#include "RendererAPI.h"
//Renderer::Submit->RenderCommand::DrawIndex->RendererAPI=OpenGLRenderAPI->OpenGLRenderAPI::SetClearColor/Clear/DrawIndex
namespace TGE {
	class RenderCommand 
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}
		inline static void SetClearColor(const glm::vec4& color) 
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void SetCullFaceFront()
		{
			s_RendererAPI->SetCullFaceFront();
		}
		inline static void SetCullFaceBack()
		{
			s_RendererAPI->SetCullFaceBack();
		}
		inline static void CloseCullFace()
		{
			s_RendererAPI->CloseCullFace();
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndex(vertexArray, count);
		}
		inline static void DrawIndexTS(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexTS(vertexArray, count);
		}
		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawLines(vertexArray, count);
		}
		inline static void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawPatches(vertexArray, count);
		}
		inline static void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawArrays(vertexArray, count);
		}
		inline static void DrawArraysTS(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawArraysTS(vertexArray, count);
		}
		inline static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}