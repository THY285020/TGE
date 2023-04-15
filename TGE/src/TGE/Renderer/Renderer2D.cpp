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
		//TODO: color,texid
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 1;
		static const uint32_t MaxVertices = MaxQuads * 4;//4个顶点
		static const uint32_t MaxIndices = MaxQuads * 6; //6个索引
		static const uint32_t MaxTextureSlots = 32;//render caps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;//顶点数组的开头
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;				//WhiteTexture占用0号

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
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
			{ ShaderDataType::Float,  "TilingFactor"}
			});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];//顶点数组

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];		//索引数组

		//uint32_t quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
		//设置索引值
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6, offset += 4)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		}

		/*Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));*/
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		//创建透明纹理
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t WhiteTextureData = 0xffffffff;//透明纹理
		s_Data.WhiteTexture->SetData(&WhiteTextureData, sizeof(uint32_t));
		//s_Data.FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");

		//因为索引值=slot，直接按索引初始化
		int samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; ++i)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture_s.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("Textures", s_Data.MaxTextureSlots, samplers);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::shutdown()
	{

	}

	void Renderer2D::BeginScene(const OrthoCamera& camera)
	{
		//s_Data.FlatColorShader->Bind();
		//s_Data.FlatColorShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("ViewProj", camera.GetViewProjectionMatrix());
		//reset
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("ViewProj", camera.GetViewProjection());
		//reset
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		//s_Data.CircleIndexCount = 0;
		//s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		//s_Data.LineVertexCount = 0;
		//s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		//s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4 transform)
	{
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("ViewProj", camera.GetProjection()*glm::inverse(transform));
		//reset
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::EndScene()
	{
		//计算顶点数
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);//此时才把数据输入VBO
		
		Flush();
	}

	void Renderer2D::SetEntity(int id)
	{
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetInt("entity_id", id);
	}
	void Renderer2D::Flush()
	{
		//BindTexture
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; ++i)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}
		//（VAO，索引数量）
		RenderCommand::DrawIndex(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}
	void Renderer2D::FlushAndReset()
	{
		Renderer2D::EndScene();
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
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

		DrawQuad(transform, color);

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


		DrawQuad(transform, color);
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
	void  Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, Ref<SubTexture2D> subtexture, const float tilingFactor)
	{
		DrawQuad(glm::vec3(pos, 0.0f), size, subtexture, tilingFactor);
	}
	//mat4
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
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
			s_Data.QuadVertexBufferPtr++;
		}
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const float tilingFactor)
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
		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
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

