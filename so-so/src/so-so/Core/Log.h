#pragma once

#include "Core.h"

// Ignore warnings from spdlog's external libraries
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace soso {

	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }


	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core logging macros
#define SS_CORE_CRITICAL(...) ::soso::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define SS_CORE_ERROR(...)    ::soso::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SS_CORE_WARN(...)     ::soso::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SS_CORE_INFO(...)     ::soso::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SS_CORE_TRACE(...)    ::soso::Log::GetCoreLogger()->trace(__VA_ARGS__)


// Client logging macros
#define SS_CRITICAL(...)      ::soso::Log::GetClientLogger()->critical(__VA_ARGS__)
#define SS_ERROR(...)	      ::soso::Log::GetClientLogger()->error(__VA_ARGS__)
#define SS_WARN(...)	      ::soso::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SS_INFO(...)	      ::soso::Log::GetClientLogger()->info(__VA_ARGS__)
#define SS_TRACE(...)	      ::soso::Log::GetClientLogger()->trace(__VA_ARGS__)