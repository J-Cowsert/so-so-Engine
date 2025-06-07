#pragma once

#include "Core.h"
#include "UID.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <filesystem>
#include <format>

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

// Custom Formatters
namespace std {

	template <>
	struct formatter<soso::UID> : formatter<uint64_t>
	{
		template <typename FormatContext>
		FormatContext::iterator format(const soso::UID id, FormatContext& ctx) const
		{
			return formatter<uint64_t>::format(static_cast<uint64_t>(id), ctx);
		}
	};


	template <>
	struct formatter<filesystem::path> : formatter<string>
	{
		template <typename FormatContext>
		FormatContext::iterator format(const filesystem::path& path, FormatContext& ctx) const
		{
			return formatter<string>::format(path.string(), ctx);
		}
	};


	template<>
	struct formatter<glm::vec2>
	{
		char presentation = 'f';

		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
		{
			auto it = ctx.begin(), end = ctx.end();
			if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

			if (it != end && *it != '}') throw format_error("invalid format");

			return it;
		}

		template <typename FormatContext>
		auto format(const glm::vec2& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? format_to(ctx.out(), "({:.3f}, {:.3f})", vec.x, vec.y)
				: format_to(ctx.out(), "({:.3e}, {:.3e})", vec.x, vec.y);
		}
	};

	template<>
	struct formatter<glm::vec3>
	{
		char presentation = 'f';

		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
		{
			auto it = ctx.begin(), end = ctx.end();
			if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

			if (it != end && *it != '}') throw format_error("invalid format");

			return it;
		}

		template <typename FormatContext>
		auto format(const glm::vec3& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", vec.x, vec.y, vec.z)
				: format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e})", vec.x, vec.y, vec.z);
		}
	};

	template<>
	struct formatter<glm::vec4>
	{
		char presentation = 'f';

		constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
		{
			auto it = ctx.begin(), end = ctx.end();
			if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

			if (it != end && *it != '}') throw format_error("invalid format");

			return it;
		}

		template <typename FormatContext>
		auto format(const glm::vec4& vec, FormatContext& ctx) const -> decltype(ctx.out())
		{
			return presentation == 'f'
				? format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f}, {:.3f})", vec.x, vec.y, vec.z, vec.w)
				: format_to(ctx.out(), "({:.3e}, {:.3e}, {:.3e}, {:.3e})", vec.x, vec.y, vec.z, vec.w);
		}
	};
}