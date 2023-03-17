#pragma once
#include <glm/gtc/type_ptr.hpp>
#include "TGE/Renderer/OrthoCameraController.h"
#include "TGE/Renderer/Texture.h"

namespace TGE {
	class Renderer2D
	{
	public:
		static void Init();
		static void shutdown();

		static void BeginScene(const OrthoCamera& camera);
		static void EndScene();

		//premitives
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture);

		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture);
	};
}


