#pragma once
#include "TGE/Core/core.h"
//#include <string>
//#include <functional>

namespace TGE {
	enum class TGE_API EventType{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
	//多个类别应用于单个事件，每个事件单独一个位
	enum EventCategory{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput =	   BIT(1),
		EventCategoryKeyboard =    BIT(2),
		EventCategoryMouse =	   BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

//这些宏用于实现Event的纯虚函数
//#转化，将其后面的东西转化为字符串 ##连接，将前后连接形成一个词
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::##type;}\
								virtual EventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class TGE_API Event {
			friend class EventDispatcher;
		public:
			//EVENT_CLASS_TYPE
			virtual EventType GetEventType() const = 0;
			virtual const char* GetName() const = 0;
			//EVENT_CLASS_CATEGORY
			virtual int GetCategoryFlags() const = 0;

			virtual std::string ToString() const { return GetName(); }

			inline bool IsInCategory(EventCategory category) {
				return GetCategoryFlags() & category;
			}
			bool m_Handled = false;//用来标记这个事件是否被处理了
	};
	
	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event):m_Event(event){}

		template<typename T>
		bool Dispatch(EventFn<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}
}

