#include "tgpch.h"
#include "RenderCommand.h"

#include "Platform/Opengl/OpenGLRendererAPI.h"

namespace TGE {
	//������static�ı���Ҫ���������޶�
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}

