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

		virtual void SetInt(const std::string& name, const int& value) override;
		virtual void SetIntArray(const std::string& name, uint32_t count, int* values) override;
		virtual void SetFloat(const std::string& name, const int& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }

		void SetUniformInt(const std::string& name, const int values);
		void SetUniformIntArray(const std::string& name, uint32_t count, int* values);
		void SetUniformFloat(const std::string& name, const float values);
		void SetUniformFloat2(const std::string& name, const glm::vec2& values);
		void SetUniformFloat3(const std::string& name, const glm::vec3& values);
		void SetUniformFloat4(const std::string& name, const glm::vec4& values);

		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
		void SetUniformMat3(const std::string& name, const glm::mat3& matrix);

		virtual uint32_t GetID()const override{ return Shader_ID; }
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		
	private:
		uint32_t Shader_ID;
		std::string m_Name;
	};
}


