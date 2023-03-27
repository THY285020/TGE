#include "tgpch.h"
#include "TGE.h"
#include "TGE/Core/EntryPoint.h"

#include "Platform/Opengl/OpenGLShader.h"
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include "EditorLayer.h"

class TGEEditor :public TGE::Application {
public:
	TGEEditor() : Application("TGE Editor")
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new TGE::EditorLayer());
	}
	~TGEEditor() {

	}
};

TGE::Application* TGE::CreateApplication() {
	return new TGEEditor();
}