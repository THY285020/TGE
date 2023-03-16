#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "Shader.h"
#include "Platform/Opengl/OpenGLShader.h"
#include "OrthoCamera.h"

namespace TGE {

	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(OrthoCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::vec3& pos);
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::vec3& pos, const glm::vec3& scal);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

}