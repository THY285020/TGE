#pragma once
#include "TGE/Renderer/FrameBuffer.h"

namespace TGE
{
	class OpenGLFrameBuffer: public FrameBuffer
	{
	public:
		OpenGLFrameBuffer();
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; };
		
		virtual uint32_t GetColorAttachment(uint32_t index = 0) const override { TGE_CORE_ASSERT(index < m_ColorAttachments.size(), "ColorAttachments' size overflow!"); return m_ColorAttachments[index]; };
		virtual void SetColorAttachment(uint32_t textureID, uint32_t attachment) override;

		virtual uint32_t GetCubeAttachment(uint32_t index)override;
		virtual void SetCubeAttachment(uint32_t attachment, uint32_t face, uint32_t index)override;


	private:
		//m_Specification-AttachmentSpec-TextureSpec-TextureFormat
		uint32_t m_RendererID = 0;
		
		FrameBufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpec;
		FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureSpecification();
		std::vector<FramebufferTextureSpecification> m_CubeAttachmentsSpec;

		std::vector<uint32_t> m_ColorAttachments;
		std::vector<uint32_t> m_CubeAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}


