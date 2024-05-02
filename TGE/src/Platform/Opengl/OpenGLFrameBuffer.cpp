#include "tgpch.h"
#include "OpenGLFrameBuffer.h"
#include <glad/glad.h>
namespace TGE
{
	static const uint32_t MaxFramebufferSize = 8192;
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)//判断是否使用多重采样
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}
		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
			//glGenTextures(count, outID);
			//glBindTexture(TextureTarget(multisampled), *outID);
		}

		static void CreateCubeTextures(uint32_t* ID, uint32_t count)
		{
			glCreateTextures(GL_TEXTURE_CUBE_MAP, count, ID);
		}

		static void BindCubeTexture(uint32_t id)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		}
		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}
		static void AttachColorTexture(uint32_t id, uint32_t samples, GLenum internalformat, GLenum format, uint32_t width, uint32_t height,int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}
		static void SetCubeTexture(uint32_t id, uint32_t samples, GLenum internalformat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			for (int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
			}
		}
		static void AttachDepthTexture(uint32_t id, uint32_t samples, GLenum format, GLenum type, uint32_t width, uint32_t height)
		{
			//type = GL_DEPTH_STENCIL_ATTACHMENT; format = GL_DEPTH24_STENCIL8
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
				//glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			//attach depth to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, type, TextureTarget(multisampled), id, 0);
		}
		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}
		static bool IsColorFormat(FramebufferTextureFormat format)
		{
			if (format >= FramebufferTextureFormat::RGB8 && format <= FramebufferTextureFormat::RED_INT)
			{
				return true;
			}
			return false;
		}
		static bool IsCubeFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::Cube_RGB8: return true;
			}
			return false;
		}
		static GLenum FBTextureFormat2GL(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGB8:
				return GL_RGB8;
			case FramebufferTextureFormat::RGBA8:
				return GL_RGBA8;
			case FramebufferTextureFormat::RGB16F:
				return GL_RGB16F;
			case FramebufferTextureFormat::RED_INT:
				return GL_RED_INTEGER;
			}
			return 0;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer()
	{
		glGenFramebuffers(1, &m_RendererID);
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		//将m_Spec中的texture spec传到本类的color/depth spec
		for (auto ts : m_Specification.Attachments.TextureSpecifications)
		{
			if (Utils::IsColorFormat(ts.TextureFormat))
				m_ColorAttachmentSpec.emplace_back(ts);
			else if (Utils::IsCubeFormat(ts.TextureFormat))
				m_CubeAttachmentsSpec.emplace_back(ts);
			else if(Utils::IsDepthFormat(ts.TextureFormat))
				m_DepthAttachmentSpec = ts;
		}
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_CubeAttachments.clear();
			m_DepthAttachment = 0;
		}
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		//Attachments
		//Color
		if (m_ColorAttachmentSpec.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpec.size());//resize初始化时分配id
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpec[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGB8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGB8, GL_RGB, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RGB16F:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGB16F, GL_RGB, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INT:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}

		}
		//Cube
		if (m_CubeAttachmentsSpec.size())
		{
			m_CubeAttachments.resize(m_CubeAttachmentsSpec.size());
			Utils::CreateCubeTextures(m_CubeAttachments.data(), m_CubeAttachments.size());
			for (size_t i = 0; i < m_CubeAttachments.size(); ++i)
			{
				Utils::SetCubeTexture(m_CubeAttachments[i], m_Specification.Samples, GL_RGB8, GL_RGB, m_Specification.Width, m_Specification.Height, i);
			}
		}

		//Depth
		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpec.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1 || m_CubeAttachments.size() > 1)//其中一个留给ID，所以要>1
		{
			TGE_CORE_ASSERT(m_ColorAttachments.size() <= 4, "m_ColorAttachments > 4!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(4, buffers);
		}
		else if(m_ColorAttachments.empty() && m_CubeAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}
#pragma region origin
		//Color------------------------------------------------------
		/*glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);*/
		//glGenTextures(1, &m_ColorAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 
		//	0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		////attach texture to framebuffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
		////Depth------------------------------------------------------
		//glGenTextures(1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height,
		//	0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		////attach depth to framebuffer
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
#pragma endregion
		TGE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		//TGE_TRACE(glCheckFramebufferStatus(GL_FRAMEBUFFER));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);

		//Clear Attachments
		//int value = -1;
		//(texture level format type value)
		//glClearTexImage(m_ColorAttachments[1], 0, GL_RED_INTEGER, GL_INT, &value);
		//ClearAttachment(1, -1);
	}

	void OpenGLFrameBuffer::SetCubeAttachment(uint32_t i, uint32_t face, uint32_t index)
	{
		TGE_CORE_ASSERT(i >= 0 && i<=5, "Error CubeMap Index!");
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_CubeAttachments[index], 0);
	}

	uint32_t OpenGLFrameBuffer::GetCubeAttachment(uint32_t index)
	{
		return m_CubeAttachments[index];
	}

	void OpenGLFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
		{
			TGE_CORE_WARN("Attempted to Resize FrameBuffer to ({0}, {1}) !", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		TGE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "ColorAttachments' size overflow in ReadPixel!");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		//x, y, width, height format type data
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		TGE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "ColorAttachments' size overflow in ClearAttachment!");
		auto& spec = m_ColorAttachmentSpec[attachmentIndex];

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, 
			Utils::FBTextureFormat2GL(spec.TextureFormat),
			GL_INT, &value);
	}

	void OpenGLFrameBuffer::SetColorAttachment(uint32_t textureID, uint32_t attachment)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+attachment, GL_TEXTURE_2D, textureID, 0);
	}

}