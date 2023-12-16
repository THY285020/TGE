#include "tgpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace TGE
{
	OpenGLTexture2D::OpenGLTexture2D()
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		TGE_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
		//s和t轴的包围方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//GL_REPEAT GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		TGE_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported!");

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		
		//OpenGL（4.4和ES 3.0）使用glTexStorage2D分配空间，再用glTexSubImage2D传入数据
		//指定纹理目标 多级渐远纹理级别 存储格式 宽度 高度 历史遗留0 格式 组成图像的数据类型 数据 
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, NULL);

		//线性插值Linear平滑模糊 Nearest锐利锯齿
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
		//s和t轴的包围方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//GL_REPEAT GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		TGE_CORE_ASSERT(data, "Failed to load image!");

		m_Width = width;
		m_Height = height;
		
		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		TGE_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		//指定纹理目标 多级渐远纹理级别 存储格式 宽度 高度 历史遗留0 格式 组成图像的数据类型 数据 
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		//线性插值Linear平滑模糊 Nearest锐利锯齿
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
		//s和t轴的包围方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_REPEAT GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(data);//释放内存
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		TGE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		//更新纹理数据
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
		//glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	//---------------Cube------------------------
	OpenGLTextureCube::OpenGLTextureCube()
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
		//s和t轴的包围方式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//GL_REPEAT GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::array<std::string, 6>& path)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		for (unsigned int i = 0; i < 6; ++i)
		{
			if (path[i] == "")
			{
				uint32_t data = 0xffffffff;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data);
			}
			else
			{
				stbi_uc* data = stbi_load(path[i].c_str(), &width, &height, &channels, 0);
				TGE_CORE_ASSERT(data, "Failed to load image!");

				if (channels == 1)
				{
					m_InternalFormats[i] = m_DataFormats[i] = GL_RED;
				}
				else if (channels == 3)
				{
					m_InternalFormats[i] = gammaCorrection ? GL_SRGB : GL_RGB;
					m_InternalFormats[i] = GL_RGB;
					m_DataFormats[i] = GL_RGB;
				}
				else if (channels == 4)
				{
					m_InternalFormats[i] = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
					m_InternalFormats[i] = GL_RGBA;
					m_DataFormats[i] = GL_RGBA;
				}
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormats[i], width, height, 0, m_DataFormats[i], GL_UNSIGNED_BYTE, data);
				m_Widthes[i] = width;
				m_Heights[i] = height;
				stbi_image_free(data);
			}
		}
			
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST
		//s和t轴的包围方式
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//GL_REPEAT GL_CLAMP_TO_EDGE
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}


	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
	}

	void OpenGLTextureCube::SetData(void* data, uint32_t size)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}

	void OpenGLTextureCube::SetDataI(int i, const std::string& path)
	{
		TGE_CORE_ASSERT(i >= 0 && i <= 6, "Failed to load image!");

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		TGE_CORE_ASSERT(data, "Failed to load image!");
		if (channels == 1)
		{
			m_InternalFormats[i] = m_DataFormats[i] = GL_RED;
		}
		else if (channels == 3)
		{
			m_InternalFormats[i] = gammaCorrection ? GL_SRGB : GL_RGB;
			m_InternalFormats[i] = GL_RGB;
			m_DataFormats[i] = GL_RGB;
		}
		else if (channels == 4)
		{
			m_InternalFormats[i] = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			m_InternalFormats[i] = GL_RGBA;
			m_DataFormats[i] = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormats[i], width, height, 0, m_DataFormats[i], GL_UNSIGNED_BYTE, data);

	}

	//void OpenGLTextureCube::SetData(void* data, uint32_t size)
	//{
	//	for (unsigned int i = 0; i < 6; ++i)
	//	{
	//		uint32_t bpp = m_DataFormats[i] == GL_RGBA ? 4 : 3;
	//		TGE_CORE_ASSERT(size == m_Widthes[i] * m_Heights[i] * bpp, "Data must be entire texture!");
	//		//更新纹理数据
	//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
	//		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormats[i], m_Widthes[i], m_Heights[i], 0, m_DataFormats[i], GL_UNSIGNED_BYTE, data);
	//		//glTexSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	//	}
	//	
	//}

}
