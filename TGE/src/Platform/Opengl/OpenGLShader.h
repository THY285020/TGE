#pragma once
#include "TGE/Renderer/Shader.h"
#include <glm/glm.hpp>

//TODO:REMOVE
typedef unsigned int GLenum;

namespace TGE{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		virtual ~OpenGLShader();
		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		void SetUniformInt(const std::string& name, const int values);

		void SetUniformFloat(const std::string& name, const float values);
		void SetUniformFloat2(const std::string& name, const glm::vec2& values);
		void SetUniformFloat3(const std::string& name, const glm::vec3& values);
		void SetUniformFloat4(const std::string& name, const glm::vec4& values);

		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
		void SetUniformMat3(const std::string& name, const glm::mat3& matrix);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		
	private:
		uint32_t Shader_ID;
		std::string m_Name;
	};
}


