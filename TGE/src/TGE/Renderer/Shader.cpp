#include "tgpch.h"
#include "Shader.h"

//#include <glad/glad.h>
//#include <glm/gtc/type_ptr.hpp>
#include "Renderer.h"
#include "Platform/Opengl/OpenGLShader.h"

namespace TGE {

	Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return new OpenGLShader(filepath);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


}

