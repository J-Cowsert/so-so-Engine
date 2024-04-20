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

#define BIT(x) (1 << x)