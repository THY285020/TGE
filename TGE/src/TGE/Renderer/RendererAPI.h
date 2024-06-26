#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "VertexArray.h"

namespace TGE {
	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetCullFaceFront() = 0;
		virtual void SetCullFaceBack() = 0;
		virtual void CloseCullFace() = 0;
		virtual void Clear() = 0;

		virtual void DrawIndex(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0) = 0;
		virtual void DrawIndexTS(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexcount = 0) = 0;
		virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexcount = 0) = 0;
		virtual void DrawArraysTS(const Ref<VertexArray>& vertexArray, uint32_t vertexcount = 0) = 0;
		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t vertexcount) = 0;

		virtual void SetLineWidth(float thickness) = 0;
		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}