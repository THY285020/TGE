#include "tgpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/Opengl/OpenGLTexture.h"

namespace TGE
{
	Ref<Texture2D> TGE::Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
