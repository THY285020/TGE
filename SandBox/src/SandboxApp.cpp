#include "tgpch.h"
#include "TGE.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>

#include "imgui/imgui.h"

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public TGE::Layer {
public:
	ExampleLayer():Layer("Example")
	{
		auto cam = camera(5.0f, { 0.5f, 0.5f });
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnUpdate() override
	{
		//TGE_INFO("ExampleLayer::Update");
		if (TGE::Input::IsKeyPressed(TGE_KEY_TAB)) {
			TGE_INFO("Tab key is pressed!(poll)");
		}	
	}

	void OnEvent(TGE::Event& event) override
	{
		//TGE_TRACE("{0}", event);
		if (event.GetEventType() == TGE::EventType::KeyPressed) {
			TGE::KeyPressedEvent& e = (TGE::KeyPressedEvent&)event;
			TGE_TRACE("{0}", (char)e.GetKeyCode());
		}

	}
};

class Sandbox :public TGE::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		/*PushOverlay(new TGE::ImGuiLayer());*/
	}
	~Sandbox() {

	}
};

TGE::Application* TGE::CreateApplication() {
	return new Sandbox();
}