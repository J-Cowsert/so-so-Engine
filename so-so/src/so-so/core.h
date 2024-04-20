#pragma once

#ifdef SS_PLATFORM_WINDOWS
	#ifdef SS_BUILD_DLL
		#define SOSO_API __declspec(dllexport)
	#else
		#define SOSO_API __declspec(dllimport)
	#endif // SS_BUILD

#else
#error soso only supports Windows :(
#endif // SS_PLATFORM_WINDOWS

#ifdef SS_CORE_ASSERTS
	#define HZ_ASSERT(x, ...) { if(!(x)) { {SS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { {SS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SS_ASSERT(x, ...)
	#define SS_CORE_ASSERT(x, ...)
#endif // SS_CORE_ASSERTS

#define BIT(x) (1 << x)