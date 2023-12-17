#pragma once
#include "Shader.h"
#include "VertexArray.h"
#include "TGE/Renderer/EditorCamera.h"

namespace TGE
{
	//struct TerrainData
	//{
	//	glm::vec3 pos;
	//	glm::vec2 uv;
	//	glm::vec3 color;
	//};
	//class EditorCamera;
	//class Terrain
	//{
	//public:
	//	Terrain() = default;
	//	void Begin(const EditorCamera& camera);
	//	void Init();
	//	void Draw();

	//private:
	//	Ref<VertexArray> terrainVAO;
	//	Ref<VertexBuffer> terrainVBO;
	//	Ref<IndexBuffer> terrainIBO;
	//	Ref<Shader> terrainShader;
	//	TerrainData m_Data[4];
	//};


	class EditorCamera;
	class Grid
	{
	public:
		
		static void Begin(const EditorCamera& camera);
		static void Init();
		static void Draw();
	};
}


