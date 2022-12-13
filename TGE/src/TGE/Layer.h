#pragma once

#include "TGE/core.h"
#include "TGE/Events/Event.h"
#include "TGE/Core/TimeStep.h"

namespace TGE {
	class TGE_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep& ts) {}
		virtual void OnImGuiRender();
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}