#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace soso {

	// Unique Identifier - Randomly generated 64 bit integer.

	class UID {
	public:
		UID();
		UID(std::uint64_t uid);

		operator std::uint64_t() const noexcept { return m_UID; }

	private:
		std::uint64_t m_UID;
	};
}

namespace std {

	template<>
	struct hash<soso::UID> {

		std::size_t operator()(const soso::UID& uid) const noexcept {
			return std::hash<std::uint64_t>{}(static_cast<std::uint64_t>(uid));
		}
	};
}
