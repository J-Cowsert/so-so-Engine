#pragma once

#include <memory>

#ifdef SS_PLATFORM_WINDOWS
#if SS_DYNAMIC_LINKING

	#ifdef SS_BUILD_DLL
		#define SOSO_API __declspec(dllexport)
	#else
		#define SOSO_API __declspec(dllimport)
	#endif // SS_BUILD

#else
	#define SOSO_API
#endif // SS_DYNAMIC_LINKING
#else
#error soso only supports Windows :(
#endif // SS_PLATFORM_WINDOWS

#ifdef SS_DEBUG
	#define SS_CORE_ASSERTS
#endif

#ifdef SS_CORE_ASSERTS
	#define SS_ASSERT(x, ...) { if(!(x)) { SS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SS_CORE_ASSERT(x, ...) { if(!(x)) { SS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SS_ASSERT(x, ...)
	#define SS_CORE_ASSERT(x, ...)
#endif // SS_CORE_ASSERTS

#define BIT(x) (1 << x)
#define SS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace soso {

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
template<typename T>
using Scope = std::unique_ptr<T>;
