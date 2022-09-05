#pragma once
#include<string>
#include <glm/glm.hpp>

namespace TGE {
	class Shader {
	public:
		Shader(const std::string& vertexSrc, const std::string fragmentSrc);
		~Shader();
		void Bind() const;
		void UnBind() const;

		void SetUniformMat4(const std::string& name,const glm::mat4& matrix);
	private:
		uint32_t Shader_ID;
	};
}