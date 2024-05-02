#pragma once
namespace TGE
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		
		//color
		RGB8,
		RGBA8,
		RGB16F,
		RED_INT,

		//Cube
		Cube_RGB8,
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
	//Specification °üº¬ AttachmentsSpecification °üº¬TextureSpecification
	class FrameBuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		virtual void SetColorAttachment(uint32_t textureID, uint32_t attachment) = 0;
		virtual uint32_t GetColorAttachment(uint32_t index) const = 0;

		virtual void SetCubeAttachment(uint32_t attachment, uint32_t face, uint32_t index) = 0;
		virtual uint32_t GetCubeAttachment(uint32_t index) = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
		static Ref<FrameBuffer> Create();
	};

}

