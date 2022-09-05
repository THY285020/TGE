#include "tgpch.h"
#include "RenderCommand.h"

#include "Platform/Opengl/OpenGLRendererAPI.h"

namespace TGE {
	//声明了static的变量要有作用域限定
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}

