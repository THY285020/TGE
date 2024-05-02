#pragma once
#include "TGE/Renderer/Texture.h"
#include <glad/glad.h>
#include <array>

namespace TGE
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D();
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const  std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_TextureID; }
		virtual std::string GetPath()const override { return m_Path; }

		virtual void Bind(uint32_t slot) const override;
		virtual void UnBind() const override { glBindTexture(GL_TEXTURE_2D, 0); }

		virtual void GenMipmap() const override;

		virtual void SetData(void* data, uint32_t size) override;

		virtual bool operator ==(const Texture& other) const override 
		{ 
			return m_TextureID == ((OpenGLTexture2D&)other).m_TextureID;
		};


	private:
		std::string m_Path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_TextureID;
		GLenum m_InternalFormat;
		GLenum m_DataFormat;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube();
		//OpenGLTextureCube(uint32_t width, uint32_t height);
		OpenGLTextureCube(const std::array<std::string, 6>& path);
		virtual ~OpenGLTextureCube();

		virtual uint32_t GetWidth() const override { return 0; }
		virtual uint32_t GetHeight() const override { return 0; }
		virtual uint32_t GetRendererID() const override { return m_TextureID; }
		virtual std::string GetPath()const override { return "CubeTexture"; }

		virtual void Bind(uint32_t slot) const override;
		virtual void UnBind() const override { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); };

		virtual void GenMipmap() const override;

		virtual void SetData(void* data, uint32_t size) override;

		void SetDataI(int index, const std::string& path);

		virtual bool operator ==(const Texture& other) const override
		{
			return m_TextureID == ((OpenGLTextureCube&)other).m_TextureID;
		};

	private:
		//std::string m_Path;
		uint32_t m_TextureID;
		std::array<uint32_t, 6> m_Widthes;
		std::array<uint32_t, 6> m_Heights;
		std::array<GLenum, 6> m_InternalFormats;
		std::array<GLenum, 6> m_DataFormats;
		bool gammaCorrection = false;
	};
}


