#include "tgpch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include"TGE/Renderer/RenderCommand.h"
#include <glm/gtc/type_ptr.hpp>

namespace TGE
{

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		glm::vec4 Color;
		float TexIndex;
		int ID;
	};

	struct SphereVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec3 Normal;
		glm::vec4 Color;
		float TexIndex;
		int ID;
	};

	struct Renderer3DData
	{

		static const uint32_t CubeVertexCount = 24;
		static const uint32_t MaxCubes = 200;		
		static const uint32_t MaxCubeIndices = MaxCubes * 36; //索引
		static const uint32_t MaxTextureSlots = 32;//render caps	

		const float PI = 3.14159265359f;
		static const uint32_t X_Segments = 64;
		static const uint32_t Y_Segments = 64;
		static const uint32_t SphereVertexCount = (X_Segments + 1) * (Y_Segments + 1);
		static const uint32_t MaxSpheres = 2;
		static const uint32_t MaxSphereIndices = MaxSpheres * (X_Segments + 1) * Y_Segments * 2; //索引 MaxSpheres * (X_Segments + 1) * Y_Segments * 2

		static const uint32_t MaxCubeVertices = MaxCubes * CubeVertexCount;//顶点

		static const uint32_t MaxSphereVertices = MaxSpheres * SphereVertexCount;
		
		Ref<Texture> WhiteTexture;
		//Cube
		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> CubeShader;

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		//Sphere
		Ref<VertexArray> SphereVertexArray;
		Ref<VertexBuffer> SphereVertexBuffer;
		Ref<Shader> SphereShader;

		uint32_t SphereIndexCount = 0;
		SphereVertex* SphereVertexBufferBase = nullptr;
		SphereVertex* SphereVertexBufferPtr = nullptr;

		float LineWidth = 1.0f;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;				//WhiteTexture占用0号

		glm::vec4 CubeVertexPositions[CubeVertexCount];
		glm::vec2 CubeTexCoords[CubeVertexCount];
		glm::vec3 CubeNormals[CubeVertexCount];

		glm::vec4 SphereVertexPositions[SphereVertexCount];
		glm::vec2 SphereTexCoords[SphereVertexCount];
		glm::vec3 SphereNormals[SphereVertexCount];

		Renderer3D::Statistics Stats;

	};

	static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
		//Cube---------------------------------------------------
		s_Data.CubeVertexArray = VertexArray::Create();
		s_Data.CubeVertexBuffer = VertexBuffer::Create(s_Data.MaxCubeVertices * sizeof(CubeVertex));//申请空间
		s_Data.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float2, "aTexCoord" },
			{ ShaderDataType::Float3, "aNormal" },
			{ ShaderDataType::Float4, "aColor" },
			{ ShaderDataType::Float,  "TexIndex" },
			{ ShaderDataType::Int,    "ID"}
			});
		s_Data.CubeVertexArray->AddVertexBuffer(s_Data.CubeVertexBuffer);
		s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxCubeVertices];

		//Sphere------------------------------------------------
		s_Data.SphereVertexArray = VertexArray::Create();
		s_Data.SphereVertexBuffer = VertexBuffer::Create(s_Data.MaxSphereVertices * sizeof(SphereVertex));
		s_Data.SphereVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float2, "aTexCoord" },
			{ ShaderDataType::Float3, "aNormal" },
			{ ShaderDataType::Float4, "aColor" },
			{ ShaderDataType::Float,  "TexIndex" },
			{ ShaderDataType::Int,    "ID"}
			});
		s_Data.SphereVertexArray->AddVertexBuffer(s_Data.SphereVertexBuffer);
		s_Data.SphereVertexBufferBase = new SphereVertex[s_Data.MaxSphereVertices];

		//CubeIndex
		uint32_t* cubeIndices = new uint32_t[s_Data.MaxCubeIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxCubeIndices; i += 36, offset += 24)
		{
			cubeIndices[i + 0] = offset + 3;
			cubeIndices[i + 1] = offset + 2;
			cubeIndices[i + 2] = offset + 1;
			cubeIndices[i + 3] = offset + 1;
			cubeIndices[i + 4] = offset + 0;
			cubeIndices[i + 5] = offset + 3;

			cubeIndices[i + 6] = offset + 4;
			cubeIndices[i + 7] = offset + 5;
			cubeIndices[i + 8] = offset + 6;
			cubeIndices[i + 9] = offset + 6;
			cubeIndices[i + 10] = offset + 7;
			cubeIndices[i + 11] = offset + 4;

			cubeIndices[i + 12] = offset + 8;
			cubeIndices[i + 13] = offset + 9;
			cubeIndices[i + 14] = offset + 10;
			cubeIndices[i + 15] = offset + 10;
			cubeIndices[i + 16] = offset + 11;
			cubeIndices[i + 17] = offset + 8;

			cubeIndices[i + 18] = offset + 13;
			cubeIndices[i + 19] = offset + 12;
			cubeIndices[i + 20] = offset + 15;
			cubeIndices[i + 21] = offset + 15;
			cubeIndices[i + 22] = offset + 14;
			cubeIndices[i + 23] = offset + 13;

			cubeIndices[i + 24] = offset + 16;
			cubeIndices[i + 25] = offset + 19;
			cubeIndices[i + 26] = offset + 18;
			cubeIndices[i + 27] = offset + 18;
			cubeIndices[i + 28] = offset + 17;
			cubeIndices[i + 29] = offset + 16;

			cubeIndices[i + 30] = offset + 20;
			cubeIndices[i + 31] = offset + 21;
			cubeIndices[i + 32] = offset + 22;
			cubeIndices[i + 33] = offset + 22;
			cubeIndices[i + 34] = offset + 23;
			cubeIndices[i + 35] = offset + 20;
		}

		Ref<IndexBuffer> cubeIB = IndexBuffer::Create(cubeIndices, s_Data.MaxCubeIndices);
		s_Data.CubeVertexArray->SetIndexBuffer(cubeIB);
		delete[] cubeIndices;

		//SphereIndex
		std::vector<uint32_t> sphereIndices;
		std::vector<uint32_t> tmp;
		bool oddRow = false;
		offset = 0;

		for (unsigned int y = 0; y < s_Data.Y_Segments; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2;
			{
				for (unsigned int x = 0; x <= s_Data.X_Segments; ++x)
				{
					tmp.push_back(y * (s_Data.X_Segments + 1) + x);
					tmp.push_back((y + 1) * (s_Data.X_Segments + 1) + x);
				}
			}
			else
			{
				for (int x = s_Data.X_Segments; x >= 0; --x)
				{
					tmp.push_back((y + 1) * (s_Data.X_Segments + 1) + x);
					tmp.push_back(y * (s_Data.X_Segments + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		//uint32_t indexCount = static_cast<unsigned int>(tmp.size());
		//Ref<IndexBuffer> sphereIB = IndexBuffer::Create(&tmp[0], indexCount);
		//s_Data.SphereVertexArray->SetIndexBuffer(sphereIB);

		for (uint32_t i = 0; i < s_Data.MaxSpheres; i++, offset += s_Data.SphereVertexCount)
		{
			for (auto& x : tmp)
			{
				x += offset;
			}
			sphereIndices.insert(sphereIndices.end(), tmp.begin(), tmp.end());
		}

		Ref<IndexBuffer> sphereIB = IndexBuffer::Create(&sphereIndices[0], s_Data.MaxSphereIndices);
		s_Data.SphereVertexArray->SetIndexBuffer(sphereIB);

		tmp.clear();
		tmp.shrink_to_fit();
		sphereIndices.clear();
		sphereIndices.shrink_to_fit();

		
		//SphereVertexData-------------------------------------------------------------------
		for (uint32_t x = 0; x <= s_Data.X_Segments; ++x)
		{
			for (uint32_t y = 0; y <= s_Data.Y_Segments; ++y)
			{
				float phi = float(x) / float(s_Data.X_Segments);
				float xita = float(y) / float(s_Data.Y_Segments);
				float xPos = std::cos(2 * s_Data.PI * phi) * std::sin(s_Data.PI * xita);
				float yPos = std::cos(s_Data.PI * xita);
				float zPos = std::sin(2 * s_Data.PI * phi) * std::sin(s_Data.PI * xita);
				//0-Y, Y+1-2Y+1, 2Y+2-3Y+2 (X)*(Y+1)-X*Y+X+Y=(X+1)*(Y+1)-1
				s_Data.SphereVertexPositions[x * (s_Data.Y_Segments+1) + y] = glm::vec4(xPos, yPos, zPos, 1.0f);
				s_Data.SphereTexCoords[x * (s_Data.Y_Segments + 1) + y] = glm::vec2(phi, -xita);//从顶部往下，x轴方向开始逆时针
				s_Data.SphereNormals[x * (s_Data.Y_Segments + 1) + y] = glm::vec3(xPos, yPos, zPos);
			}
		}

		//创建透明纹理
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t WhiteTextureData = 0xffffffff;//透明纹理
		s_Data.WhiteTexture->SetData(&WhiteTextureData, sizeof(uint32_t));
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		//创建Shader
		s_Data.CubeShader = Shader::Create("assets/shaders/Cube.glsl");
		s_Data.SphereShader = Shader::Create("assets/shaders/Sphere.glsl");


		//--------------------CubeVertexData-----------------------
			//背面
			s_Data.CubeVertexPositions[0] = {-0.5f, -0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[1] = { 0.5f, -0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[2] = { 0.5f,  0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[3] = {-0.5f,  0.5f, -0.5f, 1.0f};
			//正面					
			s_Data.CubeVertexPositions[4] = {-0.5f, -0.5f, 0.5f, 1.0f};
			s_Data.CubeVertexPositions[5] = { 0.5f, -0.5f, 0.5f, 1.0f};
			s_Data.CubeVertexPositions[6] = { 0.5f,  0.5f, 0.5f, 1.0f};
			s_Data.CubeVertexPositions[7] = {-0.5f,  0.5f, 0.5f, 1.0f};
			//左边							  		 
			s_Data.CubeVertexPositions[8] = {-0.5f, -0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[9] = {-0.5f, -0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[10] = {-0.5f,  0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[11] = {-0.5f,  0.5f, -0.5f, 1.0f};
			//右边					
			s_Data.CubeVertexPositions[12] = {0.5f, -0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[13] = {0.5f, -0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[14] = {0.5f,  0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[15] = {0.5f,  0.5f, -0.5f, 1.0f};
			//底面		
			s_Data.CubeVertexPositions[16] = {-0.5f, -0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[17] = {-0.5f, -0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[18] = { 0.5f, -0.5f,  0.5f, 1.0f};
			s_Data.CubeVertexPositions[19] = { 0.5f, -0.5f, -0.5f, 1.0f};
			//顶面							   0.5
			s_Data.CubeVertexPositions[20] = {-0.5f, 0.5f, -0.5f, 1.0f};
			s_Data.CubeVertexPositions[21] = {-0.5f, 0.5f, 0.5f, 1.0f};
			s_Data.CubeVertexPositions[22] = { 0.5f, 0.5f, 0.5f, 1.0f};
			s_Data.CubeVertexPositions[23] = { 0.5f, 0.5f, -0.5f, 1.0f};

				s_Data.CubeTexCoords[0] = { 0.0f, 1.0f };
				s_Data.CubeTexCoords[1] = { 1.0f, 1.0f };
				s_Data.CubeTexCoords[2] = { 1.0f, 0.0f };
				s_Data.CubeTexCoords[3] = { 0.0f, 0.0f };

				s_Data.CubeTexCoords[4] = { 0.0f, 0.0f };
				s_Data.CubeTexCoords[5] = { 1.0f, 0.0f };
				s_Data.CubeTexCoords[6] = { 1.0f, 1.0f };
				s_Data.CubeTexCoords[7] = { 0.0f, 1.0f };

				s_Data.CubeTexCoords[8] = { 0.0f, 0.0f };
				s_Data.CubeTexCoords[9] = { 1.0f, 0.0f };
				s_Data.CubeTexCoords[10] = { 1.0f, 1.0f };
				s_Data.CubeTexCoords[11] = { 0.0f, 1.0f };

				s_Data.CubeTexCoords[12] = { 1.0f, 0.0f };
				s_Data.CubeTexCoords[13] = { 0.0f, 0.0f };
				s_Data.CubeTexCoords[14] = { 0.0f, 1.0f };
				s_Data.CubeTexCoords[15] = { 1.0f, 1.0f };

				s_Data.CubeTexCoords[16] = { 0.0f, 0.0f };
				s_Data.CubeTexCoords[17] = { 0.0f, 1.0f };
				s_Data.CubeTexCoords[18] = { 1.0f, 1.0f };
				s_Data.CubeTexCoords[19] = { 1.0f, 0.0f };

				s_Data.CubeTexCoords[20] = { 0.0f, 1.0f };
				s_Data.CubeTexCoords[21] = { 0.0f, 0.0f };
				s_Data.CubeTexCoords[22] = { 1.0f, 0.0f };
				s_Data.CubeTexCoords[23] = { 1.0f, 1.0f };

				s_Data.CubeNormals[0] = { 0.0f,  0.0f, -1.0f };
				s_Data.CubeNormals[1] = { 0.0f,  0.0f, -1.0f };
				s_Data.CubeNormals[2] = { 0.0f,  0.0f, -1.0f };
				s_Data.CubeNormals[3] = { 0.0f,  0.0f, -1.0f };

				s_Data.CubeNormals[4] = { 0.0f,  0.0f, 1.0f };
				s_Data.CubeNormals[5] = { 0.0f,  0.0f, 1.0f };
				s_Data.CubeNormals[6] = { 0.0f,  0.0f, 1.0f };
				s_Data.CubeNormals[7] = { 0.0f,  0.0f, 1.0f };
						   
				s_Data.CubeNormals[8] = { -1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[9] = { -1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[10] = { -1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[11] = { -1.0f,  0.0f, 0.0f };
						   
				s_Data.CubeNormals[12] = { 1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[13] = { 1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[14] = { 1.0f,  0.0f, 0.0f };
				s_Data.CubeNormals[15] = { 1.0f,  0.0f, 0.0f };
						   
				s_Data.CubeNormals[16] = { 0.0f, -1.0f, 0.0f };
				s_Data.CubeNormals[17] = { 0.0f, -1.0f, 0.0f };
				s_Data.CubeNormals[18] = { 0.0f, -1.0f, 0.0f };
				s_Data.CubeNormals[19] = { 0.0f, -1.0f, 0.0f };
						   
				s_Data.CubeNormals[20] = { 0.0f, 1.0f, 0.0f };
				s_Data.CubeNormals[21] = { 0.0f, 1.0f, 0.0f };
				s_Data.CubeNormals[22] = { 0.0f, 1.0f, 0.0f };
				s_Data.CubeNormals[23] = { 0.0f, 1.0f, 0.0f };
	}

	void Renderer3D::shutdown()
	{

	}

	void Renderer3D::BeginScene(const OrthoCamera& camera)
	{
		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.SphereShader->Bind();
		s_Data.SphereShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.SphereIndexCount = 0;
		s_Data.SphereVertexBufferPtr = s_Data.SphereVertexBufferBase;
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetViewProjection());
		//reset
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.SphereShader->Bind();
		s_Data.SphereShader->SetMat4("ViewProj", camera.GetViewProjection());
		//reset
		s_Data.SphereIndexCount = 0;
		s_Data.SphereVertexBufferPtr = s_Data.SphereVertexBufferBase;
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4 transform)
	{
		s_Data.CubeShader->Bind();
		s_Data.CubeShader->SetMat4("ViewProj", camera.GetProjection() * glm::inverse(transform));
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.SphereShader->Bind();
		s_Data.SphereShader->SetMat4("ViewProj", camera.GetProjection() * glm::inverse(transform));
		s_Data.SphereIndexCount = 0;
		s_Data.SphereVertexBufferPtr = s_Data.SphereVertexBufferBase;
	}

	void Renderer3D::EndScene()
	{
		Flush();
	}

	void Renderer3D::Flush()
	{
	
		if (s_Data.CubeIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase;
			s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, dataSize);//此时才把数据输入VBO
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}
			s_Data.CubeShader->Bind();
			RenderCommand::DrawIndex(s_Data.CubeVertexArray, s_Data.CubeIndexCount);
			s_Data.Stats.DrawCalls++;
		}


		if (s_Data.SphereIndexCount)
		{
			uint32_t dataSize = (uint8_t*)s_Data.SphereVertexBufferPtr - (uint8_t*)s_Data.SphereVertexBufferBase;
			s_Data.SphereVertexBuffer->SetData(s_Data.SphereVertexBufferBase, dataSize);//此时才把数据输入VBO
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
			{
				s_Data.TextureSlots[i]->Bind(i);
			}
			s_Data.SphereShader->Bind();
			RenderCommand::DrawIndexTS(s_Data.SphereVertexArray, s_Data.SphereIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}
	void Renderer3D::FlushAndReset()
	{
		Renderer3D::EndScene();
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.SphereIndexCount = 0;
		s_Data.SphereVertexBufferPtr = s_Data.SphereVertexBufferBase;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, Ref<Texture2D> texture, int entity_id)
	{
		if (s_Data.CubeIndexCount >= Renderer3DData::MaxCubeIndices)
			FlushAndReset();

		float texIndex = 0.0f;
		
		//第一轮传入texture后，便不再需要传递texture
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			//TextureSlots[i]已经有且TextureSlots[i]==要使用的texture，此时找到了texture所在slot
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture（上述操作没改变，说明未传入slot）
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint32_t i = 0; i < s_Data.CubeVertexCount; ++i)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * s_Data.CubeVertexPositions[i];
			s_Data.CubeVertexBufferPtr->TexCoord = s_Data.CubeTexCoords[i];
			s_Data.CubeVertexBufferPtr->Normal = s_Data.CubeNormals[i];
			s_Data.CubeVertexBufferPtr->Color = color;
			s_Data.CubeVertexBufferPtr->TexIndex = texIndex;
			s_Data.CubeVertexBufferPtr->ID = entity_id;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.CubeIndexCount += 36;
		s_Data.Stats.CubeCount++;
	}
	
	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entity_id=-1)
	{
		if (s_Data.CubeIndexCount >= Renderer3DData::MaxCubeIndices)
			FlushAndReset();

		float texIndex = 0.0f;

		//constexpr int CubeVertexCount = 24;
		//constexpr glm::vec2 TexCoords[CubeVertexCount] =
		//{
		//	{ 0.0f, 1.0f},
		//	{ 1.0f, 1.0f},
		//	{ 1.0f, 0.0f},
		//	{ 0.0f, 0.0f},

		//	{ 0.0f, 0.0f},
		//	{ 1.0f, 0.0f},
		//	{ 1.0f, 1.0f},
		//	{ 0.0f, 1.0f},

		//	{ 0.0f, 0.0f},
		//	{ 1.0f, 0.0f},
		//	{ 1.0f, 1.0f},
		//	{ 0.0f, 1.0f},

		//	{ 1.0f, 0.0f},
		//	{ 0.0f, 0.0f},
		//	{ 0.0f, 1.0f},
		//	{ 1.0f, 1.0f},

		//	{ 0.0f, 0.0f},
		//	{ 0.0f, 1.0f},
		//	{ 1.0f, 1.0f},
		//	{ 1.0f, 0.0f},

		//	{ 0.0f, 1.0f},
		//	{ 0.0f, 0.0f},
		//	{ 1.0f, 0.0f},
		//	{ 1.0f, 1.0f}
		//};
		//constexpr glm::vec3 Normals[CubeVertexCount] =
		//{
		//	 {0.0f,  0.0f, -1.0f},
		//	 {0.0f,  0.0f, -1.0f},
		//	 {0.0f,  0.0f, -1.0f},
		//	 {0.0f,  0.0f, -1.0f},

		//	 {0.0f,  0.0f, 1.0f},
		//	 {0.0f,  0.0f, 1.0f},
		//	 {0.0f,  0.0f, 1.0f},
		//	 {0.0f,  0.0f, 1.0f},

		//	 {-1.0f,  0.0f, 0.0f},
		//	 {-1.0f,  0.0f, 0.0f},
		//	 {-1.0f,  0.0f, 0.0f},
		//	 {-1.0f,  0.0f, 0.0f},

		//	 {1.0f,  0.0f, 0.0f},
		//	 {1.0f,  0.0f, 0.0f},
		//	 {1.0f,  0.0f, 0.0f},
		//	 {1.0f,  0.0f, 0.0f},

		//	 {0.0f, -1.0f, 0.0f},
		//	 {0.0f, -1.0f, 0.0f},
		//	 {0.0f, -1.0f, 0.0f},
		//	 {0.0f, -1.0f, 0.0f},

		//	 {0.0f, 1.0f, 0.0f},
		//	 {0.0f, 1.0f, 0.0f},
		//	 {0.0f, 1.0f, 0.0f},
		//	 {0.0f, 1.0f, 0.0f}
		//};

		for (uint32_t i = 0; i < s_Data.CubeVertexCount; ++i)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * s_Data.CubeVertexPositions[i];
			s_Data.CubeVertexBufferPtr->TexCoord = s_Data.CubeTexCoords[i];
			s_Data.CubeVertexBufferPtr->Normal = s_Data.CubeNormals[i];//s_Data.Cube
			s_Data.CubeVertexBufferPtr->Color = color;
			s_Data.CubeVertexBufferPtr->TexIndex = texIndex;
			s_Data.CubeVertexBufferPtr->ID = entity_id;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.CubeIndexCount += 36;
		s_Data.Stats.CubeCount++;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, CubeRendererComponent& src, int entity_id=-1)
	{
		if (src.usingTexture)
		{
			DrawCube(transform, src.Color, src.Texture, entity_id);
		}
		else
		{
			DrawCube(transform, src.Color, entity_id);
		}
	}
	
	void Renderer3D::DrawSphere(const glm::mat4& transform, glm::vec4& color, int entity_id=-1)
	{
		if (s_Data.SphereIndexCount >= Renderer3DData::MaxSphereIndices)
			FlushAndReset();

		for (uint32_t i = 0; i < s_Data.SphereVertexCount; ++i)
		{
			s_Data.SphereVertexBufferPtr->Position = glm::vec3(transform * s_Data.SphereVertexPositions[i]);
			s_Data.SphereVertexBufferPtr->TexCoord = s_Data.SphereTexCoords[i];
			s_Data.SphereVertexBufferPtr->Normal = s_Data.SphereNormals[i];//s_Data.Sphere
			s_Data.SphereVertexBufferPtr->Color = color;
			s_Data.SphereVertexBufferPtr->TexIndex = 0.0f;
			s_Data.SphereVertexBufferPtr->ID = entity_id;
			s_Data.SphereVertexBufferPtr++;
		}

		s_Data.SphereShader->Bind();

		s_Data.SphereIndexCount += (s_Data.X_Segments + 1) * s_Data.Y_Segments * 2;
		s_Data.Stats.SphereCount++;
	}

	void Renderer3D::DrawSphere(const glm::mat4& transform, glm::vec4& color, Ref<Texture2D> texture, int entity_id=-1)
	{
		if (s_Data.SphereIndexCount >= Renderer3DData::MaxSphereIndices)
			FlushAndReset();

		float texIndex = 0.0f;

		//第一轮传入texture后，便不再需要传递texture
		for (uint32_t i = 1; i < s_Data.TextureSlots.size(); ++i)
		{
			//TextureSlots[i]已经有且TextureSlots[i]==要使用的texture，此时找到了texture所在slot
			if (s_Data.TextureSlots[i].get() && *s_Data.TextureSlots[i].get() == *texture.get())
			{
				texIndex = (float)i;
				break;
			}
		}
		//第一轮需要向数组传入texture（上述操作没改变，说明未传入slot）
		if (texIndex == 0.0)
		{
			texIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (uint32_t i = 0; i < s_Data.SphereVertexCount; ++i)
		{
			s_Data.SphereVertexBufferPtr->Position = transform * s_Data.SphereVertexPositions[i];
			s_Data.SphereVertexBufferPtr->TexCoord = s_Data.SphereTexCoords[i];
			s_Data.SphereVertexBufferPtr->Normal = s_Data.SphereNormals[i];
			s_Data.SphereVertexBufferPtr->Color = color;
			s_Data.SphereVertexBufferPtr->TexIndex = texIndex;
			s_Data.SphereVertexBufferPtr->ID = entity_id;
			s_Data.SphereVertexBufferPtr++;
		}

		s_Data.SphereShader->Bind();

		s_Data.SphereIndexCount += (s_Data.X_Segments + 1) * s_Data.Y_Segments * 2;
		s_Data.Stats.SphereCount++;
	}

	void Renderer3D::DrawSphere(const glm::mat4& transform, SphereRendererComponent& src, int entity_id)
	{
		if (src.usingTexture)
		{
			DrawSphere(transform, src.Color, src.Texture, entity_id);
		}
		else
		{
			DrawSphere(transform, src.Color, entity_id);
		}
	}
	
	float Renderer3D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}
	void Renderer3D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}
	//Statistics
	void Renderer3D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
	Renderer3D::Statistics Renderer3D::GetStats()
	{
		return s_Data.Stats;
	}

	Ref<VertexArray> Renderer3D::GetSphereVAO()
	{
		return s_Data.SphereVertexArray;
	}
}

