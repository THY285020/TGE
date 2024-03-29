#pragma once
#include <glm/gtc/type_ptr.hpp>
#include "TGE/Renderer/OrthoCameraController.h"
#include "TGE/Renderer/Texture.h"
#include "TGE/Renderer/SubTexture2D.h"
#include "TGE/Renderer/Camera.h"
#include "TGE/Renderer/EditorCamera.h"
#include "TGE/Scene/Component.h"
namespace TGE {
	class EditorCamera;
	class Renderer2D
	{
	public:
		static void Init();//in Application
		static void shutdown();

		static void BeginScene(const OrthoCamera& camera);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4 transform);
		static void EndScene();

		//static void SetEntityCube(int id);
		static void Flush();

		//premitives
		//Quad
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor);
		static void DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor);

		static void DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor, float rotation);
		static void DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation);
		static void DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor, float rotation);

		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor);
		static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id);
		static void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const float tilingFactor);

		static void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, glm::vec4 color, const float tilingFactor, int entity_id);
		//Line
		static void DrawLine(const glm::vec3 p0, glm::vec3 p1, const glm::vec4& color, int entity_id = -1);
		//Circle
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entity_id = -1);
		//Quad
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entity_id = -1);
		//Rectangle
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color, int entity_id = -1);
		//cube
		//static void DrawCube(const glm::mat4& transform, const glm::vec4& color, Ref<Texture2D> texture, int entity_id = -1);
		//static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entity_id);
		//static void DrawCube(const glm::mat4& transform, CubeRendererComponent& src, int entity_id);

		static float GetLineWidth();
		static void SetLineWidth(float width);
		//Statistics
		struct Statistics
		{
			uint32_t DrawCalls = 0;

			uint32_t QuadCount = 0;
			uint32_t LineCount = 0;
			uint32_t CubeCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4 + LineCount*2 + CubeCount*8; };
			uint32_t GetTotalIndexCount() { return QuadCount * 6; };
		};

		static void ResetStats();
		static Statistics GetStats();
	private:
		static void FlushAndReset();
	};
}


