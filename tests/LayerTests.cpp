#include "so-so/Core/Layer.h"

#include <doctest.h>

#include <memory>
#include <string>

TEST_SUITE("Layer") {

	TEST_CASE("Layer names are independent of caller-owned strings") {

		std::string name = "Simulation";
		soso::Layer layer(name);
		name = "Modified by caller";

		CHECK(layer.GetName() == "Simulation");
	}

	TEST_CASE("Layer names survive temporary and local strings") {

		soso::Layer temporary(std::string(256, 'x'));
		CHECK(temporary.GetName() == std::string(256, 'x'));

		auto layer = [] {
			std::string name(256, 'y');
			return std::make_unique<soso::Layer>(name);
		}();
		CHECK(layer->GetName() == std::string(256, 'y'));
	}

	TEST_CASE("Default layer names survive construction") {

		soso::Layer layer;
		CHECK(layer.GetName() == "Layer");
	}
}
