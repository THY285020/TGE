#include "tgpch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/Opengl/OpenGLFrameBuffer.h"

namespace TGE
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFrameBuffer>(spec);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
