#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "Shader.h"
#include "OrthoCamera.h"

namespace TGE {

	class Renderer
	{
	public:
		static void BeginScene(OrthoCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

}