#include "tgpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/Opengl/OpenGLTexture.h"

namespace TGE
{
	Ref<Texture2D> Texture2D::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>();
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	//Ref<Texture2D> Texture2D::Create(uint32_t id)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
	//	case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(id);
	//	}

	//	TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
	//	return nullptr;
	//}
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void Texture2D::GenMipmap(uint32_t id)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: 
			glBindTexture(GL_TEXTURE_2D, id);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	//-------------------Cube----------------------------
	Ref<TextureCube> TextureCube::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTextureCube>();
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//Ref<TextureCube> TextureCube::Create(uint32_t id)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
	//	case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTextureCube>(id);
	//	}

	//	TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
	//	return nullptr;
	//}

	Ref<TextureCube> TextureCube::Create(const std::array<std::string, 6>& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTextureCube>(path);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	void TextureCube::GenMipmap(uint32_t id)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL:
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
	}
}
