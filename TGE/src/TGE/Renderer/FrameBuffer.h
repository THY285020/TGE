#pragma once
namespace TGE
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		
		//color
		RGBA8,
		RED_INT,

		//Depth/Stencil
		DEPTH24STENCIL8,

		//Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat textureFormat)
			:TextureFormat(textureFormat) {}
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentsSpecification
	{
		FramebufferAttachmentsSpecification() = default;
		FramebufferAttachmentsSpecification(std::initializer_list<FramebufferTextureSpecification> textureS)
			:TextureSpecifications(textureS) {}
			std::vector<FramebufferTextureSpecification> TextureSpecifications;
	};

	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		//FrameBufferFormat
		FramebufferAttachmentsSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
		
	};
	class FrameBuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachment(uint32_t index) const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}

