#pragma once

#include"TGE/Layer.h"
#include"TGE/Events/KeyEvent.h"
#include"TGE/Events/MouseEvent.h"
#include"TGE/Events/ApplicationEvent.h"

namespace TGE {
	class TGE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnImGuiRender() override;
	/*	void OnUpdate();*/
	//	void OnEvent(Event& event);
	//private:
	//	bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	//	bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	//	bool OnMouseScrolledEvent(MouseScrolledEvent& e);
	//	bool OnKeyPressedEvent(KeyPressedEvent& e);
	//	bool OnKeyReleasedEvent(KeyReleasedEvent& e);
	//	bool OnKeyTypedEvent(KeyTypedEvent& e);
	//	bool OnMouseMovedEvent(MouseMovedEvent& e);
	//	bool OnWindowResizeEvent(WindowResizeEvent& e);
		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}