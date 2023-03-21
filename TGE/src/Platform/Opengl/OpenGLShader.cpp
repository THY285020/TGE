#include "tgpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>//glm::value_ptr需要

namespace TGE
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") 
			return GL_VERTEX_SHADER;
		if (type == "fragment"|| type == "pixel")
			return GL_FRAGMENT_SHADER;
		TGE_CORE_ASSERT(false, "Unknown shader type!")
		return 0;
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());//获取文件大小
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			TGE_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		return result;
	}
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)										//npos 表示不存在的位置
		{
			size_t eol = source.find_first_of("\r\n", pos);						//找到第一个换行符的位置
			TGE_CORE_ASSERT(eol != std::string::npos, "Shader syntax error!");	//找不到换行的情况
			size_t begin = pos + typeTokenLength + 1;							//类型说明开始的位置
			std::string type = source.substr(begin, eol - begin);				//获取type字符串
			TGE_CORE_ASSERT(ShaderTypeFromString(type), "Invaild shader type!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);			//从换行符找下一行开始的位置，也就是源代码开始的部分
			TGE_CORE_ASSERT(nextLinePos != std::string::npos, "Invalid shader content!");//找不到内容

			pos = source.find(typeToken, nextLinePos);							//从下一行找下一个typeToken的位置
			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLint program = glCreateProgram();
		TGE_CORE_ASSERT(shaderSources.size() <= 3, "We only support up to 3 shaders for now!");
		std::array<GLenum, 3> glShaderIDs;
		int glShaderIDIndex = 0;
		//std::vector<GLenum> glShaderIDs;//array更节省性能
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.
				TGE_CORE_ERROR("{0}", infoLog.data());
				TGE_CORE_ASSERT(false, "Shader compilation failure!")
					// In this simple program, we'll just leave
					return;
			}
			// Attach our shaders to our program
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
			//glShaderIDs.push_back(shader);
		}

#pragma region shader
		//// Create an empty vertex shader handle
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//// Send the vertex shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//const GLchar* source = (const GLchar*)vertexSrc.c_str();
		//glShaderSource(vertexShader, 1, &source, 0);

		//// Compile the vertex shader
		//glCompileShader(vertexShader);

		//GLint isCompiled = 0;
		//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.
		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Vertex Shader compilation failure!")
		//		// In this simple program, we'll just leave
		//		return;
		//}

		//// Create an empty fragment shader handle
		//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		//// Send the fragment shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//source = (const GLchar*)fragmentSrc.c_str();
		//glShaderSource(fragmentShader, 1, &source, 0);

		//// Compile the fragment shader
		//glCompileShader(fragmentShader);

		//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(fragmentShader);
		//	// Either of them. Don't leak shaders.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.
		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Fragment Shader compilation failure!")
		//		// In this simple program, we'll just leave
		//		return;
		//}
#pragma endregion
		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		Shader_ID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}

			// Use the infoLog as you see fit.
			TGE_CORE_ERROR("{0}", infoLog.data());
			TGE_CORE_ASSERT(false, "Shader link failure!")
				// In this simple program, we'll just leave
				return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
		}
#pragma region future
		//------------------------------------------
		//GLuint program = glCreateProgram();
		//TGE_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		//std::array<GLenum, 2> glShaderIDs;
		//int glShaderIDIndex = 0;
		//for (auto& kv : shaderSources)
		//{
		//	GLenum type = kv.first;
		//	const std::string& source = kv.second;

		//	GLuint shader = glCreateShader(type);

		//	const GLchar* sourceCStr = source.c_str();
		//	glShaderSource(shader, 1, &sourceCStr, 0);

		//	glCompileShader(shader);

		//	GLint isCompiled = 0;
		//	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		//	if (isCompiled == GL_FALSE)
		//	{
		//		GLint maxLength = 0;
		//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//		std::vector<GLchar> infoLog(maxLength);
		//		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		//		glDeleteShader(shader);

		//		TGE_CORE_ERROR("{0}", infoLog.data());
		//		TGE_CORE_ASSERT(false, "Shader compilation failure!");
		//		break;
		//	}

		//	glAttachShader(program, shader);
		//	glShaderIDs[glShaderIDIndex++] = shader;
		//}

		//Shader_ID = program;

		//// Link our program
		//glLinkProgram(program);

		//// Note the different functions here: glGetProgram* instead of glGetShader*.
		//GLint isLinked = 0;
		//glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		//if (isLinked == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the program anymore.
		//	glDeleteProgram(program);

		//	for (auto id : glShaderIDs)
		//		glDeleteShader(id);

		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Shader link failure!");
		//	return;
		//}
		//for (auto id : glShaderIDs)
		//{
		//	glDetachShader(program, id);
		//	glDeleteShader(id);
		//}
#pragma endregion
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		//从filepath中提取文件名
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash+1;//+1是用于区分是否存在斜杠
		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - last_slash : lastDot - last_slash;//名字长度
		m_Name = filepath.substr(last_slash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
#pragma region old
		//// Create an empty vertex shader handle
		//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//// Send the vertex shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//const GLchar* source = (const GLchar*)vertexSrc.c_str();
		//glShaderSource(vertexShader, 1, &source, 0);

		//// Compile the vertex shader
		//glCompileShader(vertexShader);

		//GLint isCompiled = 0;
		//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.
		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Vertex Shader compilation failure!")
		//		// In this simple program, we'll just leave
		//		return;
		//}

		//// Create an empty fragment shader handle
		//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		//// Send the fragment shader source code to GL
		//// Note that std::string's .c_str is NULL character terminated.
		//source = (const GLchar*)fragmentSrc.c_str();
		//glShaderSource(fragmentShader, 1, &source, 0);

		//// Compile the fragment shader
		//glCompileShader(fragmentShader);

		//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		//if (isCompiled == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the shader anymore.
		//	glDeleteShader(fragmentShader);
		//	// Either of them. Don't leak shaders.
		//	glDeleteShader(vertexShader);

		//	// Use the infoLog as you see fit.
		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Fragment Shader compilation failure!")
		//		// In this simple program, we'll just leave
		//		return;
		//}

		//// Vertex and fragment shaders are successfully compiled.
		//// Now time to link them together into a program.
		//// Get a program object.
		//Shader_ID = glCreateProgram();
		//GLuint program = Shader_ID;

		//// Attach our shaders to our program
		//glAttachShader(program, vertexShader);
		//glAttachShader(program, fragmentShader);

		//// Link our program
		//glLinkProgram(program);

		//// Note the different functions here: glGetProgram* instead of glGetShader*.
		//GLint isLinked = 0;
		//glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		//if (isLinked == GL_FALSE)
		//{
		//	GLint maxLength = 0;
		//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//	// The maxLength includes the NULL character
		//	std::vector<GLchar> infoLog(maxLength);
		//	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//	// We don't need the program anymore.
		//	glDeleteProgram(program);
		//	// Don't leak shaders either.
		//	glDeleteShader(vertexShader);
		//	glDeleteShader(fragmentShader);

		//	// Use the infoLog as you see fit.
		//	TGE_CORE_ERROR("{0}", infoLog.data());
		//	TGE_CORE_ASSERT(false, "Shader link failure!")
		//		// In this simple program, we'll just leave
		//		return;
		//}

		//// Always detach shaders after a successful link.
		//glDetachShader(program, vertexShader);
		//glDetachShader(program, fragmentShader);
#pragma endregion	
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(Shader_ID);
	}
	void OpenGLShader::Bind() const
	{
		glUseProgram(Shader_ID);
	}
	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}


	void OpenGLShader::SetInt(const std::string& name, const int& value)
	{
		SetUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, uint32_t count, int* values)
	{
		SetUniformIntArray(name, count, values);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		SetUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		SetUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		SetUniformMat4(name, value);
	}

	//----------------------------------------------Uniform------------------------------------------------
	//----------------------------------------------int
	void OpenGLShader::SetUniformInt(const std::string& name, const int value)
	{
		glUniform1i(glGetUniformLocation(Shader_ID, name.c_str()), value);
	}
	void OpenGLShader::SetUniformIntArray(const std::string& name, uint32_t count, int* values)
	{
		GLint location = glGetUniformLocation(Shader_ID, name.c_str());
		glUniform1iv(location, count, values);
	}
	//----------------------------------------------float
	void OpenGLShader::SetUniformFloat(const std::string& name, const float value)
	{
		glUniform1f(glGetUniformLocation(Shader_ID, name.c_str()), value);
	}
	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUniform2fv(glGetUniformLocation(Shader_ID, name.c_str()), 1, glm::value_ptr(values));
	}
	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUniform3fv(glGetUniformLocation(Shader_ID, name.c_str()), 1, glm::value_ptr(values));
	}
	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUniform4fv(glGetUniformLocation(Shader_ID, name.c_str()), 1, glm::value_ptr(values));
	}
	//------------------------------------------------mat
	void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(Shader_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(Shader_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}




}