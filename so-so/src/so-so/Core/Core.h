#pragma once

#include <memory>

#if !defined(SS_PLATFORM_WINDOWS) && !defined(SS_PLATFORM_LINUX)
	#error Platform not supported
#endif

#if defined(__GNUC__)
	#if defined(__clang__)
		#define SS_COMPILER_CLANG
	#else
		#define SS_COMPILER_GCC
	#endif
#elif defined(_MSC_VER)
	#define SS_COMPILER_MSVC
#endif

#if defined(SS_COMPILER_CLANG)
	#define SS_FORCE_INLINE __attribute__((always_inline))
#elif defined(SS_COMPILER_GCC)
	#define SS_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(SS_COMPILER_MSVC)
	#define SS_FORCE_INLINE __forceinline
#endif

#if defined(SS_PLATFORM_WINDOWS)
	#define SS_DEBUGBREAK() __debugbreak()

#elif defined(SS_PLATFORM_LINUX)
	#include <signal.h>
	#define SS_DEBUGBREAK() raise(SIGTRAP)
#endif

#include "Log.h"

#ifdef SS_DEBUG
	#define SS_CORE_ASSERTS
#endif

#ifdef SS_CORE_ASSERTS
	#define SS_ASSERT(x, ...) { if(!(x)) { SS_ERROR("Assertion Failed: {0}", __VA_ARGS__); SS_DEBUGBREAK(); } }
	#define SS_CORE_ASSERT(x, ...) { if(!(x)) { SS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SS_DEBUGBREAK(); } }
#else
	#define SS_ASSERT(x, ...)
	#define SS_CORE_ASSERT(x, ...)
#endif // SS_CORE_ASSERTS

#define BIT(x) (1 << x)
#define SS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
