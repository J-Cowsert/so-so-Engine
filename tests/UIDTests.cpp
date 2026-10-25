#include "so-so/Core/UID.h"

#include <doctest.h>

#include <array>
#include <limits>
#include <string>
#include <unordered_map>

TEST_SUITE("UID") {

	TEST_CASE("Explicit IDs preserve the full unsigned 64-bit value") {

		const std::array<std::uint64_t, 3> values{0, 1, std::numeric_limits<std::uint64_t>::max()};
		for (const auto value : values) {

			const soso::UID id(value);
			CHECK(static_cast<std::uint64_t>(id) == value);
			const soso::UID copy = id;
			CHECK(static_cast<std::uint64_t>(copy) == value);
		}
	}

	TEST_CASE("IDs retrieve values by identity in an unordered map") {

		std::unordered_map<soso::UID, std::string> resources;
		resources.emplace(soso::UID(42), "mesh");
		resources.emplace(soso::UID(std::numeric_limits<std::uint64_t>::max()), "texture");

		CHECK(resources.at(soso::UID(42)) == "mesh");
		CHECK(resources.at(soso::UID(std::numeric_limits<std::uint64_t>::max())) == "texture");
		CHECK(resources.find(soso::UID(7)) == resources.end());
	}
}
