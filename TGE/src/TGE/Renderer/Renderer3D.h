#pragma once
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
	class Renderer3D
	{
	public:
		static void Init();
		static void shutdown();

		static void BeginScene(const OrthoCamera& camera);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4 transform);
		static void EndScene();

		static void Flush();

		//premitives
		
		//cube
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, Ref<Texture2D> texture, int entity_id = -1);
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entity_id);

		static void DrawCube(const glm::mat4& transform, CubeRendererComponent& src, int entity_id);
		//Sphere
		static void DrawSphere(const glm::mat4& transform, glm::vec4& color, int entity_id);
		static void DrawSphere(const glm::mat4& transform, glm::vec4& color, Ref<Texture2D> texture, int entity_id);
		
		static void DrawSphere(const glm::mat4& transform, SphereRendererComponent& src, int entity_id);

		
		static float GetLineWidth();
		static void SetLineWidth(float width);
		//Statistics
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t CubeCount = 0;
			uint32_t SphereCount = 0;

			uint32_t GetTotalVertexCount() { return CubeCount * 8 + SphereCount*65*65; }
			uint32_t GetTotalIndexCount() { return CubeCount * 24 + SphereCount*65*64*2; }
		};

		static void ResetStats();
		static Statistics GetStats();

		static Ref<VertexArray> GetSphereVAO();

	private:
		static void FlushAndReset();
	};
}


