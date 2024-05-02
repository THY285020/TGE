#pragma once
#include "Shader.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "EditorCamera.h"

namespace TGE
{
	class SkyRender
	{
	public:
		void Init(float width, float height);
		void Render(EditorCamera& camera);
		void Resize(float width, float height);
	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Texture;
		uint32_t m_CubeTextureID;

		Ref<FrameBuffer> m_FBO;
		Ref<FrameBuffer> m_CubeFBO;
	};
}


