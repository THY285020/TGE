#include "tgpch.h"
#include "Terrain.h"
#include"TGE/Renderer/RenderCommand.h"

#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	void Terrain::Begin(const EditorCamera& camera)
	{
		terrainShader->Bind();
		terrainShader->SetMat4("ViewProj", camera.GetViewProjection());
	}

	void Terrain::Init()
	{
		terrainShader = Shader::Create("assets/shaders/TerrainMesh.glsl");
		
		terrainVAO = VertexArray::Create();
		terrainVBO = VertexBuffer::Create(sizeof(TerrainData));//ÉêÇë¿Õ¼ä

		terrainVBO->SetLayout({
			 { ShaderDataType::Float3, "aPos"},
			 { ShaderDataType::Float2, "aTexCoord"},
			 { ShaderDataType::Float3, "aColor" }
			});

		terrainVAO->AddVertexBuffer(terrainVBO);
		glm::vec3 PosData[4] =
		{
			{-0.5, -0.5, 0.0},
			{ 0.5, -0.5, 0.0},
			{ 0.5,  0.5, 0.0},
			{-0.5,  0.5, 0.0}
		};
		glm::vec2 UVData[4]=
		{
			{0.0,  0.0},
			{1.0,  0.0},
			{1.0,  1.0},
			{0.0,  1.5}
		};

		glm::vec4 color = glm::vec4(1.0f);

		int i = 0;
		while (i < 4)
		{
			m_Data[i].pos = PosData[i];
			m_Data[i].uv = UVData[i];
			m_Data[i].color = color;
			++i;
		}
	}

	void Terrain::Draw()
	{
		terrainVBO->SetData(m_Data, 4);
		terrainShader->Bind();
		RenderCommand::DrawArraysTS(terrainVAO, 4);
	}



}