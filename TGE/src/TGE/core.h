#pragma once

#ifdef TGE_PLATFORM_WINDOWS
	#if TGE_DYNAMIC_LINK
		#ifdef TGE_BUILD_DLL
			#define TGE_API __declspec(dllexport)
		#else
			#define TGE_API __declspec(dllimport)
		#endif
	#else
		#define TGE_API
	#endif
#else
	#error TGE only support Windows!
#endif

#ifdef TGE_DEBUG
	#define TGE_ENABLE_ASSERTS
#endif	

#ifdef TGE_ENABLE_ASSERTS
	#define TGE_ASSERT(x, ...) {if(!(x)){TGE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define TGE_CORE_ASSERT(x, ...) {if(!(x)){TGE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define TGE_ASSERT(x, ...)
	#define TGE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define TGE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)