#include "catch.hpp"

#include "bitmap.hpp"

using namespace allocator;


constexpr auto SIZE = 8u;
constexpr auto SIZE_IN_BITS = 8 * SIZE;
unsigned char memory[SIZE];


TEST_CASE("BitMap special methods", "[bit map][special methods]")
{
    SECTION("default ctor creates empty map")
    {
        const auto map = BitMap{};

        CHECK(map.is_empty());
        CHECK(map.get_size() == 0);
    }

    SECTION("ctor from memory")
    {
        SECTION("empty map is created from nullptr")
        {
            const auto map = BitMap(nullptr, 0);

            REQUIRE(map.is_empty());
        }

        SECTION("empty map is created when size is 0")
        {
            const auto map = BitMap(memory, 0);

            REQUIRE(map.is_empty());
        }

        SECTION("bits are initialised when size is positive")
        {
            const auto map = BitMap(memory, SIZE_IN_BITS, true);

            CHECK_FALSE(map.is_empty());
            REQUIRE(map.get_size() == SIZE_IN_BITS);
            REQUIRE(map.at(0));
        }

    }

    SECTION("move ctor")
    {
        SECTION("from empty map")
        {
            auto empty_map = BitMap{};
            const auto map = std::move(empty_map);

            CHECK(empty_map.is_empty());
            CHECK(map.is_empty());
        }

        SECTION("from non-empty map")
        {
            auto moved_from = BitMap(memory, SIZE_IN_BITS);
            const auto moved_into = std::move(moved_from);

            CHECK(moved_from.is_empty());
            CHECK(moved_into.get_size() == SIZE_IN_BITS);
        }

    }

    SECTION("move assignment")
    {
        SECTION("empty to empty")
        {
            auto lhs = BitMap{};
            auto rhs = BitMap{};

            lhs = std::move(rhs);

            CHECK(lhs.is_empty());
            CHECK(rhs.is_empty());
        }

        SECTION("empty to non-empty")
        {
            auto lhs = BitMap(memory, SIZE_IN_BITS);
            auto rhs = BitMap{};

            lhs = std::move(rhs);

            CHECK(lhs.is_empty());
            CHECK(rhs.is_empty());
        }

        SECTION("non-empty to empty")
        {
            auto lhs = BitMap{};
            auto rhs = BitMap(memory, SIZE_IN_BITS);

            lhs = std::move(rhs);

            CHECK(rhs.is_empty());
            CHECK(lhs.get_size() == SIZE_IN_BITS);
        }

        SECTION("non-empty to non-empty")
        {
            const auto rhs_size = SIZE_IN_BITS / 4;
            auto lhs = BitMap(memory, SIZE_IN_BITS / 2);
            auto rhs = BitMap(memory + (SIZE / 2), rhs_size, true);

            lhs = std::move(rhs);

            CHECK(rhs.is_empty());
            CHECK(lhs.get_size() == rhs_size);
            CHECK(lhs.at(0));
        }

    }

}


TEST_CASE("bit manipulation", "[bit map]")
{
    auto map = BitMap(memory, SIZE_IN_BITS);
    
    map.flip(1);

    CHECK_FALSE(map.at(0));
    CHECK(map.at(1));
}


TEST_CASE("BitMap size related methods", "[bit map][size]")
{
    const auto empty_map = BitMap{};
    const auto map = BitMap(memory, SIZE_IN_BITS);

    CHECK(empty_map.is_empty());
    CHECK(empty_map.get_size() == 0);
    CHECK_FALSE(map.is_empty());
    CHECK(map.get_size() == SIZE_IN_BITS);
}
