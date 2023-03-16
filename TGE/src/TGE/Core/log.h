#pragma once

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"//在事件中记录自定义类型

namespace TGE {
	class TGE_API log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		log();
		~log();
	};
}

//... =  __VA_ARGS__
//#define 可变参数列表(...) 可变参数宏(__VA_ARGS__)
//或者#define LOG2(args...) func1(args) 

//Core log macros
#define TGE_CORE_TRACE(...) ::TGE::log::GetCoreLogger()->trace(__VA_ARGS__);
#define TGE_CORE_INFO(...)  ::TGE::log::GetCoreLogger()->info(__VA_ARGS__);
#define TGE_CORE_WARN(...)	::TGE::log::GetCoreLogger()->warn(__VA_ARGS__);
#define TGE_CORE_ERROR(...)	::TGE::log::GetCoreLogger()->error(__VA_ARGS__);
#define TGE_CORE_FATAL(...)	::TGE::log::GetCoreLogger()->critical(__VA_ARGS__);

//Client log macros
#define TGE_TRACE(...) ::TGE::log::GetClientLogger()->trace(__VA_ARGS__);
#define TGE_INFO(...)  ::TGE::log::GetClientLogger()->info(__VA_ARGS__);
#define TGE_WARN(...)	::TGE::log::GetClientLogger()->warn(__VA_ARGS__);
#define TGE_ERROR(...)	::TGE::log::GetClientLogger()->error(__VA_ARGS__);
#define TGE_FATAL(...)	::TGE::log::GetClientLogger()->critical(__VA_ARGS__);