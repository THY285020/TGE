#pragma once

#include "TGE/Renderer/RendererAPI.h"

namespace TGE {
	class OpenGLRendererAPI:public RendererAPI
	{
	public:
		virtual void Init()override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void SetCullFaceFront() override;
		virtual void SetCullFaceBack() override;
		virtual void CloseCullFace() override;
		virtual void Clear() override;

		virtual void DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexcount) override;
		virtual void DrawIndexTS(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexcount) override;
		virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexcount) override;
		virtual void DrawArraysTS(const Ref<VertexArray>& vertexArray, uint32_t vertexcount) override;
		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t vertexcount) override;

		virtual void SetLineWidth(float thickness) override;
	};
}