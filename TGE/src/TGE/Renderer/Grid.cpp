#include "tgpch.h"
#include "Grid.h"
#include"TGE/Renderer/RenderCommand.h"

#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	//void Terrain::Begin(const EditorCamera& camera)
	//{
	//	terrainShader->Bind();
	//	terrainShader->SetMat4("ViewProj", camera.GetViewProjection());
	//}

	//void Terrain::Init()
	//{
	//	terrainShader = Shader::Create("assets/shaders/TerrainMesh.glsl");
	//	
	//	terrainVAO = VertexArray::Create();
	//	terrainVBO = VertexBuffer::Create(sizeof(TerrainData));//ÉêÇë¿Õ¼ä

	//	terrainVBO->SetLayout({
	//		 { ShaderDataType::Float3, "aPos"},
	//		 { ShaderDataType::Float2, "aTexCoord"},
	//		 { ShaderDataType::Float3, "aColor" }
	//		});
	//	terrainVAO->AddVertexBuffer(terrainVBO);

	//	unsigned int indices[4] = { 0, 1, 3, 2 };
	//	terrainIBO = IndexBuffer::Create(indices, 4);
	//	terrainVAO->SetIndexBuffer(terrainIBO);

	//	glm::vec3 PosData[4] =
	//	{
	//		{-0.5, -0.5, 0.0},
	//		{ 0.5, -0.5, 0.0},
	//		{ 0.5,  0.5, 0.0},
	//		{-0.5,  0.5, 0.0}
	//	};
	//	glm::vec2 UVData[4]=
	//	{
	//		{0.0,  0.0},
	//		{1.0,  0.0},
	//		{1.0,  1.0},
	//		{0.0,  1.0}
	//	};

	//	glm::vec3 color = glm::vec3(1.0f);

	//	int i = 0;
	//	while (i < 4)
	//	{
	//		m_Data[i].pos = PosData[i];
	//		m_Data[i].uv = UVData[i];
	//		m_Data[i].color = color;
	//		++i;
	//	}


	//}

	//void Terrain::Draw()
	//{
	//	terrainVAO->Bind();
	//	terrainVBO->SetData(m_Data, 4);
	//	terrainShader->Bind();
	//	RenderCommand::DrawIndexTS(terrainVAO, 4);
	//}

	struct GridData
	{
		Ref<VertexArray> GridVAO;
		Ref<VertexBuffer> GridVBO;
		Ref<IndexBuffer> GridIBO;
		Ref<Shader> GridShader;
	};
	static GridData tData;

	void Grid::Init()
	{
		tData.GridShader = Shader::Create("assets/shaders/Grid.glsl");

		float verticesMesh[] =
		{
			-1.0, -1.0, 0.0	, 0.0, 0.0, 0.5f,0.5f,0.5f,
			 1.0, -1.0, 0.0	, 1.0, 0.0, 0.5f,0.5f,0.5f,
			 1.0,  1.0, 0.0	, 1.0, 1.0, 0.5f,0.5f,0.5f,
			-1.0,  1.0, 0.0 , 0.0, 1.0, 0.5f,0.5f,0.5f
		};

		tData.GridVAO = VertexArray::Create();
		tData.GridVBO = VertexBuffer::Create(verticesMesh, sizeof(verticesMesh));

		tData.GridVBO->SetLayout({
			 { ShaderDataType::Float3, "aPos"},
			 { ShaderDataType::Float2, "aTexCoord"},
			 { ShaderDataType::Float3, "aColor" }
			});
		tData.GridVAO->AddVertexBuffer(tData.GridVBO);

		unsigned int indices[4] = { 0, 1, 3, 2 };
		tData.GridIBO = IndexBuffer::Create(indices, 4);
		tData.GridVAO->SetIndexBuffer(tData.GridIBO);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -2.0, 0.0));
		transform =	glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		transform = glm::scale(transform, glm::vec3(100, 100, 100));

		tData.GridShader->Bind();
		tData.GridShader->SetMat4("Transform", transform);
	}

	void Grid::Begin(const EditorCamera& camera)
	{
		tData.GridShader->Bind();
		tData.GridShader->SetMat4("ViewProj", camera.GetViewProjection());
	}

	void Grid::Draw()
	{
		tData.GridShader->Bind();
		RenderCommand::DrawIndexTS(tData.GridVAO, 4);
	}


}