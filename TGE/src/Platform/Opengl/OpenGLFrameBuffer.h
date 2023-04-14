#pragma once
#include "TGE/Renderer/FrameBuffer.h"

namespace TGE
{
	class OpenGLFrameBuffer: public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; };
		virtual uint32_t GetColorAttachment(uint32_t index = 0) const override { TGE_CORE_ASSERT(index < m_ColorAttachments.size(), "ColorAttachments' size overflow!"); return m_ColorAttachments[index]; };
	private:
		//m_Specification-AttachmentSpec-TextureSpec-TextureFormat
		uint32_t m_RendererID = 0;
		
		FrameBufferSpecification m_Specification;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpec;
		FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureSpecification();

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}


