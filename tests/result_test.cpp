#include <catch2/catch.hpp>

#include "ara/core/result.h"

struct ErrorTest
{

};

struct ValueTest
{

};

TEST_CASE("Constructs a Result based on value", "[SWS_CORE], [SWS_CORE_00721]")
{
    ValueTest value;
    ara::core::Result<ValueTest, ErrorTest> result(value);

    CHECK(result.HasValue());
}