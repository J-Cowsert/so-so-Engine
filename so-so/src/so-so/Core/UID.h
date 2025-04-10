#pragma once

#include "core.h"


namespace soso {

	// Unique ID - Randomly generated 64 bit integer.

	class UID {
	public:
		UID();
		UID(uint64_t uid);

		operator uint64_t () { return m_UID; }
		operator const uint64_t () const { return m_UID; }

	private:
		uint64_t m_UID;
	};
}

namespace std {

	template<>
	struct hash<soso::UID> {

		std::size_t operator()(const soso::UID& uid) const {
			
			return uid;
		}
	};
}