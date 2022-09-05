#include "tgpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/Opengl/OpenGLBuffer.h"

//#ifdef TGE_PLATFORM_WINDOWS
//#include "Platform/Direct3D/Direct3DBuffer.h"
//#endif

namespace TGE {
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(unsigned int* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

