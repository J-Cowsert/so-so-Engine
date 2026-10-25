#include "so-so/Resource/Resource.h"

#include <doctest.h>

#include <limits>

TEST_SUITE("Resource") {

	TEST_CASE("Resource equality and inequality agree on identity") {

		const soso::Resource empty;
		soso::Resource first;
		first.ID = 42;
		soso::Resource same;
		same.ID = 42;
		soso::Resource different;
		different.ID = std::numeric_limits<std::uint64_t>::max();

		const soso::Resource& left = first;
		const soso::Resource& right = same;
		CHECK(left == right);
		CHECK_FALSE(left != right);
		CHECK_FALSE(left == different);
		CHECK(left != different);
		CHECK_FALSE(left != left);
		CHECK(empty != left);
	}
}
