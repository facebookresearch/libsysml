#include <catch2/catch.hpp>

#include "sysml/numeric.hpp"

#include <type_traits>

TEST_CASE("is_arithmetic", "")
{
    using namespace sysml;

    CHECK(is_arithmetic_v<bool>);
    CHECK(is_arithmetic_v<char>);
    CHECK(is_arithmetic_v<char8_t>);
    CHECK(is_arithmetic_v<char16_t>);
    CHECK(is_arithmetic_v<char32_t>);
    CHECK(is_arithmetic_v<wchar_t>);
    CHECK(is_arithmetic_v<short>);
    CHECK(is_arithmetic_v<int>);
    CHECK(is_arithmetic_v<long>);
    CHECK(is_arithmetic_v<long long>);

    CHECK(is_arithmetic_v<std::make_unsigned_t<char>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<char8_t>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<char16_t>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<char32_t>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<wchar_t>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<short>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<int>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<long>>);
    CHECK(is_arithmetic_v<std::make_unsigned_t<long long>>);

    CHECK(is_arithmetic_v<int8x2_t>);
    CHECK(is_arithmetic_v<uint8x2_t>);
    CHECK(is_arithmetic_v<int8x4_t>);
    CHECK(is_arithmetic_v<uint8x4_t>);
    CHECK(is_arithmetic_v<int8x8_t>);
    CHECK(is_arithmetic_v<uint8x8_t>);

    CHECK(is_arithmetic_v<int16x2_t>);
    CHECK(is_arithmetic_v<uint16x2_t>);
    CHECK(is_arithmetic_v<int16x4_t>);
    CHECK(is_arithmetic_v<uint16x4_t>);

    CHECK(is_arithmetic_v<int32x2_t>);
    CHECK(is_arithmetic_v<uint32x2_t>);

    struct A {};
    CHECK(!is_arithmetic_v<A>);

    int a = 2;
    double b = 4.0;

    a += b;

    CHECK(a == 6);
}
