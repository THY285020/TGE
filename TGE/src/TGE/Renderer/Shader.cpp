#include "tgpch.h"
#include "Shader.h"

//#include <glad/glad.h>
//#include <glm/gtc/type_ptr.hpp>
#include "Renderer.h"
#include "Platform/Opengl/OpenGLShader.h"

namespace TGE {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: TGE_CORE_ASSERT(false, "RendererAPI::None is currently support!")
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		TGE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
//-------------------------------ShaderLibrary
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		TGE_CORE_ASSERT(!Exists(name), "Shader already exists!")
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		TGE_CORE_ASSERT(!Exists(name), "Shader already exists!")
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		TGE_CORE_ASSERT(Exists(name), "Shader not found!")
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}

