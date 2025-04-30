#include "sspch.h"
#include "UID.h"
#include <random>

namespace soso {

	static std::random_device s_RandDevice;
	static std::mt19937_64 s_Engine(s_RandDevice());
	static std::uniform_int_distribution<uint64_t> s_Distribution;

	UID::UID() : m_UID(s_Distribution(s_Engine)) {}

	UID::UID(uint64_t uid) : m_UID(uid) {}
}
