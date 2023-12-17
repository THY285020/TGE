#include "tgpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include"TGE/Renderer/RenderCommand.h"
#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		float TexIndex;
		float TilingFactor;//参数越大，越密集
		int ID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;//参数越大，越密集
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		glm::vec4 Color;
		float TexIndex;
		int ID;
	};

	struct Renderer2DData
	{
		static constexpr uint32_t MaxQuads = 2000;
		static constexpr uint32_t MaxVertices = MaxQuads * 4;//4个顶点
		static constexpr uint32_t MaxIndices = MaxQuads * 4; //6个索引
		static constexpr uint32_t MaxTextureSlots = 32;//render caps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> CubeShader;
		Ref<Texture2D> CubeWhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;//顶点数组的开头
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;//顶点数组的开头
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;//顶点数组的开头
		LineVertex* LineVertexBufferPtr = nullptr;

		uint32_t CubeVertexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		float LineWidth = 1.0f;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;				//WhiteTexture占用0号

		std::array<Ref<Texture>, MaxTextureSlots> CubeTextureSlots;
		uint32_t CubeTextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];
		glm::vec4 CubeVertexPositions[36];

		Renderer2D::Statistics Stats;

		//struct CameraBuffer
		//{
		//	glm::mat4 viewProjection;
		//};
		//CameraBuffer cameraBuffer;
		//Ref<UniformBuffer> CameraUniformBuffer;

		//Ref<VertexArray> terrainVAO;
		//Ref<VertexBuffer> terrainVBO;
		//Ref<IndexBuffer> terrainIBO;
		//Ref<Shader> terrainShader;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		s_Data.QuadVertexArray = VertexArray::Create();					   
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));//申请空间
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float2, "TexCoord" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Float,  "TexIndex" },
			{ ShaderDataType::Float,  "TilingFactor"},
			{ ShaderDataType::Int,	  "ID"}
			});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];//顶点数组
		
		//Index																 //
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];		//索引数组

		//uint32_t quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//设置索引值
		//uint32_t offset = 0;
		//for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6, offset += 4)
		//{
		//	quadIndices[i + 0] = offset + 0;
		//	quadIndices[i + 1] = offset + 1;
		//	quadIndices[i + 2] = offset + 2;
		//	
		//	quadIndices[i + 3] = offset + 2;
		//	quadIndices[i + 4] = offset + 3;
		//	quadIndices[i + 5] = offset + 0;
		//}

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 4, offset += 4)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 3;
			quadIndices[i + 3] = offset + 2;
		}

		/*Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));*/
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		//Circle
		s_Data.CircleVertexArray = VertexArray::Create();
		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));//申请空间
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "WorldPosition" },
			{ ShaderDataType::Float3, "LocalPosition" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Float,  "Thickness" },
			{ ShaderDataType::Float,  "Fade"}
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);//Use quadIB
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
		
		//Lines	
		s_Data.LineVertexArray = VertexArray::Create();
		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));//申请空间
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float4, "Color" }
		});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		//Cube---------------------------------------------------
		s_Data.CubeVertexArray = VertexArray::Create();
		s_Data.CubeVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CubeVertex));//申请空间
		s_Data.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float2, "aTexCoord" },
			{ ShaderDataType::Float3, "aNormal" },
			{ ShaderDataType::Float4, "aColor" },
			{ ShaderDataType::Float,  "TexIndex" },
			{ ShaderDataType::Int,    "ID" }
			});
		s_Data.CubeVertexArray->AddVertexBuffer(s_Data.CubeVertexBuffer);
		s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxVertices];

		//创建透明纹理
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t WhiteTextureData = 0xffffffff;//透明纹理
		s_Data.WhiteTexture->SetData(&WhiteTextureData, sizeof(uint32_t));
		//Cube
		s_Data.CubeWhiteTexture = Texture2D::Create(1, 1);
		s_Data.CubeWhiteTexture->SetData(&WhiteTextureData, sizeof(uint32_t));

		//因为索引值=slot，直接按索引初始化
		//int samplers[s_Data.MaxTextureSlots];
		//for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
		//	samplers[i] = i;

		s_Data.QuadShader = Shader::Create("assets/shaders/Quad_2D.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Circle_2D.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Line_2D.glsl");
		s_Data.CubeShader = Shader::Create("assets/shaders/Cube.glsl");

		//s_Data.TextureShader->Bind();
		//s_Data.TextureShader->SetIntArray("Textures", s_Data.MaxTextureSlots, samplers);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;
		s_Data.CubeTextureSlots[0] = s_Data.CubeWhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };//0.5f,  0.5f, 0.0f, 1.0f
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };//-0.5f,  0.5f, 0.0f, 1.0f

		//--------------------Cube-----------------------
		//s_Data.CubeVertexPositions[0] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[1] = {  0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[2] = {  0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[3] = {  0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[4] = { -0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[5] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//
		//s_Data.CubeVertexPositions[6] =  { -0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[7] =  { 0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[8] =  { 0.5f,  0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[9] =  { 0.5f,  0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[10] = { -0.5f, 0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[11] = { -0.5f, -0.5f,  0.5f, 1.0f };
	
		//s_Data.CubeVertexPositions[12] = { -0.5f,  0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[13] = { -0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[14] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[15] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[16] = { -0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[17] = { -0.5f,  0.5f,  0.5f, 1.0f };
	
		//s_Data.CubeVertexPositions[18] = { 0.5f,  0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[19] = { 0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[20] = { 0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[21] = { 0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[22] = { 0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[23] = { 0.5f,  0.5f,  0.5f, 1.0f };

		//s_Data.CubeVertexPositions[18] = { 0.5f,  0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[19] = { 0.5f,  0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[20] = { 0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[21] = { 0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[22] = { 0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[23] = { 0.5f,  0.5f,  0.5f, 1.0f };
		//	
		//s_Data.CubeVertexPositions[24] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[25] = {  0.5f, -0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[26] = {  0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[27] = {  0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[28] = { -0.5f, -0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[29] = { -0.5f, -0.5f, -0.5f, 1.0f };
		//	
		//s_Data.CubeVertexPositions[30] = { -0.5f, 0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[31] = {  0.5f, 0.5f, -0.5f, 1.0f };
		//s_Data.CubeVertexPositions[32] = {  0.5f, 0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[33] = {  0.5f, 0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[34] = { -0.5f, 0.5f,  0.5f, 1.0f };
		//s_Data.CubeVertexPositions[35] = { -0.5f, 0.5f, -0.5f, 1.0f };

		//tData.terrainShader = Shader::Create("assets/shaders/TerrainMesh.glsl");

		//tData.terrainVAO = VertexArray::Create();
		//tData.terrainVBO = VertexBuffer::Create(sizeof(TerrainData));//申请

		//tData.terrainVBO->SetLayout({
		//	 { ShaderDataType::Float3, "aPos"},
		//	 { ShaderDataType::Float2, "aTexCoord"},
		//	 { ShaderDataType::Float3, "aColor" }
		//	});
		//tData.terrainVAO->AddVertexBuffer(tData.terrainVBO);

		//unsigned int indices[4] = { 0, 1, 3, 2 };
		//tData.terrainIBO = IndexBuffer::Create(indices, 4);
		//tData.terrainVAO->SetIndexBuffer(tData.terrainIBO);

		//float verticesMesh[] =
		//{
		//	-0.5, -0.5, 0.0	, 0.0, 0.0, 1.0f,1.0f,1.0f,
		//	 0.5, -0.5, 0.0	, 1.0, 0.0, 1.0f,1.0f,1.0f,
		//	 0.5,  0.5, 0.0	, 1.0, 1.0, 1.0f,1.0f,1.0f,
		//	-0.5,  0.5, 0.0 , 0.0, 1.0, 1.0f,1.0f,1.0f
		//};

		//tData.terrainVBO->SetData(verticesMesh, sizeof(verticesMesh));


	}

	void Renderer2D::shutdown()
	{

	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//future
		//s_Data.cameraBuffer.viewProjection = camera.GetViewProjectionMatrix();
		//s_Data.CameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraBuffer));
		//reset
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleShader->Bind();
		s_Data.CircleShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.CubeVertexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("ViewProj", camera.GetViewProjection());
		//s_Data.cameraBuffer.viewProjection = camera.GetViewProjectionMatrix();
		//s_Data.CameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraBuffer));
		//reset
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleShader->Bind();
		s_Data.CircleShader->SetMat4("ViewProj", camera.GetViewProjection());
		//reset
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetMat4("ViewProj", camera.GetViewProjection());
		//reset
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetViewProjection());
		s_Data.CubeShader->SetFloat3("cameraPos", camera.GetPosition());
		//reset
		s_Data.CubeVertexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		//s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4 transform)
	{
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("ViewProj", camera.GetProjection()*glm::inverse(transform));
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleShader->Bind();
		s_Data.CircleShader->SetMat4("ViewProj", camera.GetProjection() * glm::inverse(transform));
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetMat4("ViewProj", camera.GetProjection() * glm::inverse(transform));
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetProjection() * glm::inverse(transform));
		s_Data.CubeVertexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{		
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			//计算顶点数
			uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//此时才把数据输入VBO
			//BindTexture
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}
			//（VAO，索引数量）
			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexTS(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase;
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);//此时才把数据输入VBO
			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexTS(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}
		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase;
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);//此时才把数据输入VBO
			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}
		//if (s_Data.CubeVertexCount)
		//{
		//	uint32_t dataSize = (uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase;
		//	s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, dataSize);//此时才把数据输入VBO
		//	for (uint32_t i = 0; i < s_Data.CubeTextureSlotIndex; ++i)
		//	{
		//		s_Data.CubeTextureSlots[i]->Bind(i);
		//	}
		//	s_Data.CubeShader->Bind();
		//	RenderCommand::DrawArrays(s_Data.CubeVertexArray, s_Data.CubeVertexCount);
		//	s_Data.Stats.DrawCalls++;
		//}
	}
	void Renderer2D::FlushAndReset()
	{
		Renderer2D::EndScene();//Flush
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
		s_Data.CubeVertexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;
	}

	//color
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		//if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	FlushAndReset();

		//const float texIndex = 0.f;//WhiteTexture
		//const float tilingFactor = 1.0f;

		//constexpr float QuadVertexCount = 4;
		//constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color, -1);

		//更新顶点数组数据
		//for (uint32_t i = 0; i < QuadVertexCount; ++i)
		//{
		//	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		//	s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
		//	s_Data.QuadVertexBufferPtr->Color = color;
		//	s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
		//	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		//	s_Data.QuadVertexBufferPtr++;
		//}
		//
		//s_Data.QuadIndexCount += 6;//每个矩形用到三角绘制模式的6个索引
		//s_Data.Stats.QuadCount++;
#pragma region old
		//s_Data.WhiteTexture->Bind(0);
		//s_Data.TextureShader->SetFloat4("Color", color);

		//glm::mat4 transform = glm::translate(glm::mat4(1.0), pos) * 
		//	glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		//s_Data.TextureShader->SetMat4("Transfrom", transform);
		// 
		//s_Data.QuadVertexArray->Bind();
		//RenderCommand::DrawIndex(s_Data.QuadVertexArray);
#pragma endregion old
	}
	//texture
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor)
	{
		//if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	FlushAndReset();

		//constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);

		//第一轮传入texture后，便不再需要传递texture
		//constexpr float QuadVertexCount = 4;
		//constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		//float texIndex = 0.0f;
		//for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		//{
		//	if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
		//	{
		//		texIndex = (float)i;
		//		break;
		//	}
		//}
		//第一轮需要向数组传入texture
		//if (texIndex == 0.0)
		//{
		//	texIndex = (float)s_Data.TextureSlotIndex;
		//	s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		//	s_Data.TextureSlotIndex++;
		//}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor);
		//更新顶点数组数据
		/*for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;*/

		//s_Data.TextureShader->SetFloat4("Color", {1.f, 1.f, 1.f, 1.f});
		//texture->Bind(0);
		//glm::mat4 transform = glm::translate(glm::mat4(1.0), pos) *
		//	glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		//s_Data.TextureShader->SetMat4("Transfrom", transform);

		//s_Data.QuadVertexArray->Bind();
		//RenderCommand::DrawIndex(s_Data.QuadVertexArray);
		
	}
	void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const glm::vec2* TexCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);

		//第一轮传入texture后，便不再需要传递texture
		constexpr float QuadVertexCount = 4;

		float texIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		//更新顶点数组数据
		for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
	//Rotation
	void Renderer2D::DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation)
	{
		//if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	FlushAndReset();

		//const float texIndex = 0.f;
		//const float tilingFactor = 1.0f;
		//constexpr float QuadVertexCount = 4;
		//constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.f, 0.f, 1.f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });


		DrawQuad(transform, color, -1);
		/*for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;*/
	}
	void Renderer2D::DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor, float rotation)
	{
		//if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	FlushAndReset();

		//constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);
		//float texIndex = 0.0f;
		//constexpr float QuadVertexCount = 4;
		//constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		////第一轮传入texture后，便不再需要传递texture
		//for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		//{
		//	if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
		//	{
		//		texIndex = (float)i;
		//		break;
		//	}
		//}
		////第一轮需要向数组传入texture
		//if (texIndex == 0.0)
		//{
		//	texIndex = (float)s_Data.TextureSlotIndex;
		//	s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		//	s_Data.TextureSlotIndex++;
		//}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.f, 0.f, 1.f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

//		for (uint32_t i = 0; i < QuadVertexCount; ++i)
//		{
//			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
//			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
//			s_Data.QuadVertexBufferPtr->Color = color;
//			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
//			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
//			s_Data.QuadVertexBufferPtr++;
//		}
//#pragma region old
//		//s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
//		//s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
//		//s_Data.QuadVertexBufferPtr->Color = color;
//		//s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
//		//s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
//		//s_Data.QuadVertexBufferPtr++;
//
//		//s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
//		//s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
//		//s_Data.QuadVertexBufferPtr->Color = color;
//		//s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
//		//s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
//		//s_Data.QuadVertexBufferPtr++;
//
//		//s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
//		//s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
//		//s_Data.QuadVertexBufferPtr->Color = color;
//		//s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
//		//s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
//		//s_Data.QuadVertexBufferPtr++;
//
//		//s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
//		//s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
//		//s_Data.QuadVertexBufferPtr->Color = color;
//		//s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
//		//s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
//		//s_Data.QuadVertexBufferPtr++;
//#pragma endregion old
//		s_Data.QuadIndexCount += 6;
//		s_Data.Stats.QuadCount++;

		DrawQuad(transform, texture, tilingFactor);

		//s_Data.TextureShader->SetFloat4("Color", {1.f, 1.f, 1.f, 1.f});
		//texture->Bind(0);
		//glm::mat4 transform = glm::translate(glm::mat4(1.0), pos) *
		//	glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		//s_Data.TextureShader->SetMat4("Transfrom", transform);

		//s_Data.QuadVertexArray->Bind();
		//RenderCommand::DrawIndex(s_Data.QuadVertexArray);
	}
	void Renderer2D::DrawRotationQuad(const glm::vec3& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor, float rotation)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);
		float texIndex = 0.0f;
		constexpr float QuadVertexCount = 4;
		const glm::vec2* TexCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		//第一轮传入texture后，便不再需要传递texture
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.f, 0.f, 1.f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor);

		for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
	//overload
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<Texture2D> texture, const float tilingFactor)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, texture, tilingFactor);
	}
	void  Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor = 1.0f)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, subtexture, tilingFactor);
	}
	//mat4
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float texIndex = 0.f;
		const float tilingFactor = 1.0f;
		constexpr float QuadVertexCount = 4;
		constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->ID = entity_id;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 4;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const float tilingFactor = 1.0f)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);
		float texIndex = 0.0f;
		constexpr float QuadVertexCount = 4;
		constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		//第一轮传入texture后，便不再需要传递texture
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 4;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f), const float tilingFactor = 1.0f, int entity_id=-1)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		//constexpr glm::vec4 color(1.0, 1.0, 1.0, 1.0);
		float texIndex = 0.0f;
		constexpr float QuadVertexCount = 4;
		constexpr glm::vec2 TexCoords[4] = { { 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		//第一轮传入texture后，便不再需要传递texture
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint32_t i = 0; i < QuadVertexCount; ++i)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->ID = entity_id;
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 4;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawLine(const glm::vec3 p0, glm::vec3 p1, const glm::vec4& color, int entity_id)
	{

		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;
		s_Data.Stats.LineCount++;

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetInt("entity_id", entity_id);
	}
	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entity_id)
	{
		if (s_Data.CircleIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		for (uint32_t i = 0; i < 4; ++i)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr++;
		}
		s_Data.CircleIndexCount += 4;
		s_Data.Stats.QuadCount++;

		s_Data.CircleShader->Bind();
		s_Data.CircleShader->SetInt("entity_id", entity_id);
	}
	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entity_id)
	{
		if (src.usingTexture)
		{
			DrawQuad(transform, src.Texture, src.Color, src.TilingFactor, entity_id);
		}
		else
		{
			DrawQuad(transform, src.Color, entity_id);
		}

	}
	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entity_id)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; ++i)
		{
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];
		}
		DrawLine(lineVertices[0], lineVertices[1], color, entity_id);
		DrawLine(lineVertices[1], lineVertices[2], color, entity_id);
		DrawLine(lineVertices[2], lineVertices[3], color, entity_id);
		DrawLine(lineVertices[3], lineVertices[0], color, entity_id);
	}
	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color, int entity_id)
	{
		glm::vec3 p0 = glm::vec3(position.x - 0.5f * size.x, position.y - 0.5f * size.y, position.z);
		glm::vec3 p1 = glm::vec3(position.x + 0.5f * size.x, position.y - 0.5f * size.y, position.z);
		glm::vec3 p2 = glm::vec3(position.x + 0.5f * size.x, position.y + 0.5f * size.y, position.z);
		glm::vec3 p3 = glm::vec3(position.x - 0.5f * size.x, position.y + 0.5f * size.y, position.z);

		DrawLine(p0, p1, color, entity_id);
		DrawLine(p1, p2, color, entity_id);
		DrawLine(p2, p3, color, entity_id);
		DrawLine(p3, p0, color, entity_id);
	}


	//void Renderer2D::DrawCube(const glm::mat4& transform, const glm::vec4& color, Ref<Texture2D> texture, int entity_id)
	//{
	//	if (s_Data.CubeVertexCount >= Renderer2DData::MaxIndices)
	//		FlushAndReset();

	//	float texIndex = 0.0f;
	//	constexpr float CubeVertexCount = 36;
	//	constexpr glm::vec2 TexCoords[36] = 
	//	{ 
	//		{ 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 1.0f }, { 0.0f, 1.0f }, {0.0f, 0.0f},
	//		{ 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 1.0f }, { 0.0f, 1.0f }, {0.0f, 0.0f},
	//		{ 1.0f, 0.0f},{ 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 1.0f }, { 0.0f, 0.0f }, {1.0f, 0.0f},
	//		{ 1.0f, 0.0f},{ 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 1.0f }, { 0.0f, 0.0f }, {1.0f, 0.0f},
	//		{ 0.0f, 1.0f},{ 1.0f, 1.0f },{ 1.0f, 0.0f },{ 1.0f, 0.0f }, { 0.0f, 0.0f }, {0.0f, 1.0f},
	//		{ 0.0f, 1.0f},{ 1.0f, 1.0f },{ 1.0f, 0.0f },{ 1.0f, 0.0f }, { 0.0f, 0.0f }, {0.0f, 1.0f}
	//	};
	//	constexpr glm::vec3 Normals[36] =
	//	{
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},

	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},

	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},

	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},

	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},

	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f}
	//	};
	//	//第一轮传入texture后，便不再需要传递texture,在Slots中查找已有texture
	//	for (uint32_t i = 1; i < s_Data.CubeTextureSlots.size(); ++i)
	//	{
	//		//TextureSlots[i]已经有且TextureSlots[i]==要使用的texture，此时找到了texture所在slot
	//		if (s_Data.CubeTextureSlots[i].get() && *s_Data.CubeTextureSlots[i].get() == *texture.get())
	//		{
	//			texIndex = (float)i;
	//			break;
	//		}
	//	}
	//	//第一轮需要向数组传入新的texture（上述操作没改变，说明未传入slot）
	//	if (texIndex == 0.0)
	//	{
	//		texIndex = (float)s_Data.CubeTextureSlotIndex;
	//		s_Data.CubeTextureSlots[s_Data.CubeTextureSlotIndex] = texture;
	//		s_Data.CubeTextureSlotIndex++;
	//	}

	//	for (uint32_t i = 0; i < CubeVertexCount; ++i)
	//	{
	//		s_Data.CubeVertexBufferPtr->Position = transform * s_Data.CubeVertexPositions[i];
	//		s_Data.CubeVertexBufferPtr->TexCoord = TexCoords[i];
	//		s_Data.CubeVertexBufferPtr->Normal = Normals[i];
	//		s_Data.CubeVertexBufferPtr->Color = color;
	//		s_Data.CubeVertexBufferPtr->TexIndex = texIndex;
	//		s_Data.CubeVertexBufferPtr->ID = entity_id;
	//		s_Data.CubeVertexBufferPtr++;
	//	}

	//	s_Data.CubeShader->Bind();

	//	s_Data.CubeVertexCount += 36;
	//	s_Data.Stats.CubeCount++;
	//}
	//void Renderer2D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entity_id)
	//{
	//	if (s_Data.CubeVertexCount >= Renderer2DData::MaxIndices)
	//		FlushAndReset();

	//	float texIndex = 0.0f;
	//	constexpr float CubeVertexCount = 36;
	//	constexpr glm::vec2 TexCoords[36] =
	//	{
	//		{ 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 1.0f }, { 0.0f, 1.0f }, {0.0f, 0.0f},
	//		{ 0.0f, 0.0f},{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 1.0f }, { 0.0f, 1.0f }, {0.0f, 0.0f},
	//		{ 1.0f, 0.0f},{ 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 1.0f }, { 0.0f, 0.0f }, {1.0f, 0.0f},
	//		{ 1.0f, 0.0f},{ 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 1.0f }, { 0.0f, 0.0f }, {1.0f, 0.0f},
	//		{ 0.0f, 1.0f},{ 1.0f, 1.0f },{ 1.0f, 0.0f },{ 1.0f, 0.0f }, { 0.0f, 0.0f }, {0.0f, 1.0f},
	//		{ 0.0f, 1.0f},{ 1.0f, 1.0f },{ 1.0f, 0.0f },{ 1.0f, 0.0f }, { 0.0f, 0.0f }, {0.0f, 1.0f}
	//	};
	//	constexpr glm::vec3 Normals[36] =
	//	{
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},
	//		{0.0f,  0.0f, -1.0f},

	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},
	//		{0.0f,  0.0f,  1.0f},

	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},
	//		{-1.0f,  0.0f,  0.0f},

	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},
	//		{1.0f,  0.0f,  0.0f},

	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},
	//		{0.0f, -1.0f,  0.0f},

	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f},
	//		{0.0f,  1.0f,  0.0f}
	//	};

	//	for (uint32_t i = 0; i < CubeVertexCount; ++i)
	//	{
	//		s_Data.CubeVertexBufferPtr->Position = transform * s_Data.CubeVertexPositions[i];
	//		s_Data.CubeVertexBufferPtr->TexCoord = TexCoords[i];
	//		s_Data.CubeVertexBufferPtr->Normal = Normals[i];
	//		s_Data.CubeVertexBufferPtr->Color = color;
	//		s_Data.CubeVertexBufferPtr->TexIndex = texIndex;
	//		s_Data.CubeVertexBufferPtr->ID = entity_id;
	//		s_Data.CubeVertexBufferPtr++;
	//	}

	//	s_Data.CubeShader->Bind();

	//	s_Data.CubeVertexCount += 36;
	//	s_Data.Stats.CubeCount++;
	//}
	//void Renderer2D::DrawCube(const glm::mat4& transform, CubeRendererComponent& src, int entity_id)
	//{
	//	if (src.usingTexture)
	//	{
	//		DrawCube(transform, src.Color, src.Texture, entity_id);
	//	}
	//	else
	//	{
	//		DrawCube(transform, src.Color, entity_id);
	//	}
	//}


	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}
	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}
	//Statistics
	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}

