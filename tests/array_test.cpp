#include <catch2/catch.hpp>

#include "ara/core/array.h"

TEST_CASE("Array can be constructed", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 1> array{0};

    CHECK(array[0] == 0);
}

TEST_CASE("Array.at()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 1> array{0};
    CHECK(array.at(0) == 0);
}

TEST_CASE("Array operator[]", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 1> array;

    array[0] = 1;
    CHECK(array[0] == 1);
}

TEST_CASE("Array.front(), Array.back()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 2> array{0, 1};
    CHECK(array.front() == 0);
    CHECK(array.back() == 1);
}

TEST_CASE("Array.data()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 1> array{0};

    CHECK(array.data()[0] == 0);
}

TEST_CASE("Array.begin(), Array.end(), Array.rbegin(), Array.rend()",
          "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 3> array{0, 1, 2};
    int                      i = 0;
    std::for_each(array.begin(), array.end(), [&](int v) { CHECK(v == i++); });

    i--;

    std::for_each(array.rbegin(), array.rend(), [&](int v) {
        CHECK(v == i--);
    });
}

TEST_CASE("Array.empty()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 0> array;

    CHECK(array.empty() == true);
}

TEST_CASE("Array.size(). Array.max_size()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 1> array;

    CHECK(array.size() == 1);
    CHECK(array.max_size() == 1);
}

TEST_CASE("Array.fill()", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 3> array;
    array.fill(1);

    std::for_each(array.begin(), array.end(), [](int v) { CHECK(v == 1); });
}

TEST_CASE("ara::core::swap", "[SWS_CORE], [SWS_CORE_01296]")
{
    ara::core::Array<int, 3> p{0, 1, 2};
    ara::core::Array<int, 3> q{2, 1, 0};

    ara::core::swap(p, q);

    CHECK(p[0] == 2);
    CHECK(q[0] == 0);
}

TEST_CASE("operator==", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 3> a{0, 1, 2};
    ara::core::Array<int, 3> b{0, 1, 2};
    ara::core::Array<int, 3> c{2, 1, 0};

    CHECK((a == b) == true);
    CHECK((b == c) == false);
}

TEST_CASE("operator<=>", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 3> a{0, 1, 2};
    ara::core::Array<int, 3> b{0, 1, 2};
    ara::core::Array<int, 3> c{0, 1, 1};

    // This is a hack becasue Catch2 can't handle <=> properly
    // It seems that <=> is not yet supported
    if ((a <=> b) == 0)
    {
        CHECK(true);
    }
    else
    {
        CHECK(false);
    }

    CHECK((a > c) == true);
    CHECK((c < a) == true);
    CHECK((a >= b) == true);
    CHECK((b <= a) == true);
}

TEST_CASE("get<N>", "[SWS_CORE], [SWS_CORE_01201]")
{
    ara::core::Array<int, 3> a{0, 1, 2};
    CHECK(ara::core::get<1>(a) == 1);
}

TEST_CASE("to_array", "[SWS_CORE], [SWS_CORE_01201]")
{
    CHECK(ara::core::to_array({0, 1, 2}).size() == 3);
}
