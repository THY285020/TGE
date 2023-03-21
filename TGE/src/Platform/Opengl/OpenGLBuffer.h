#pragma once
#include "TGE/Renderer/Buffer.h"

namespace TGE {
	class OpenGLVertexBuffer :public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual BufferLayout GetLayout() const override{ return m_Layout;}

		virtual void SetData(const void* data, uint32_t size) override;
	private:
		uint32_t m_VertexBufferID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer :public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(unsigned int* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_IndexBufferID;
		uint32_t m_Count;
	};
}