#pragma once
#include <string>
#include "TGE/Core/core.h"

namespace TGE
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID()const = 0;
		virtual std::string GetPath()const = 0;
 		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind() const = 0;
		virtual void GenMipmap() const = 0;

		virtual bool operator ==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create();
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
		static void GenMipmap(uint32_t id);
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create();
		static Ref<TextureCube> Create(const std::array<std::string, 6>& path);
		static void GenMipmap(uint32_t id);

	};

}


