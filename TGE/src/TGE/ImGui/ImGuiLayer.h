#pragma once

#include"TGE/Core/Layer.h"
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
		//virtual void OnImGuiRender() override;
		//void OnUpdate();
	    void OnEvent(Event& e) override;
		void Begin();
		void End();
		void BlockEvents(bool b) { m_BlockEvents = b; }
		void SetThemeColor();
	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = false;
	};
}