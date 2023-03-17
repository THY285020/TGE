#include "tgpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/Opengl/OpenGLVertexArray.h"

namespace TGE {
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray> ( );
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

