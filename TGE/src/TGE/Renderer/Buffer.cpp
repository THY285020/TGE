#include "tgpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/Opengl/OpenGLBuffer.h"

//#ifdef TGE_PLATFORM_WINDOWS
//#include "Platform/Direct3D/Direct3DBuffer.h"
//#endif

namespace TGE {
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer> (vertices, size);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size);
		}
		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer> (indices, count);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

