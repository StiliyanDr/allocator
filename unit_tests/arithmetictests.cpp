#include "catch.hpp"

#include "arithmetic.hpp"

using namespace allocator;


TEST_CASE("division", "[arithmetic][division]")
{
    SECTION("simple")
    {
        const auto result = divide(3, 2);

        CHECK(result.quotient == 1);
        CHECK(result.remainder == 1);
    }

    SECTION("by bits in byte")
    {
        const auto result = divided_by_bits_in_byte(10);

        CHECK(result.quotient == 1);
        CHECK(result.remainder == 2);
    }

    SECTION("fitting units into blocks")
    {
        const auto block_size = 6u;

        CHECK(blocks_fitting(0, block_size) == 0);
        CHECK(blocks_fitting(3, block_size) == 1);
        CHECK(blocks_fitting(6, block_size) == 1);
        CHECK(blocks_fitting(11, block_size) == 2);
    }

    SECTION("converting size in bits to size in bytes")
    {
        CHECK(size_in_bytes(0) == 0);
        CHECK(size_in_bytes(7) == 1);
        CHECK(size_in_bytes(8) == 1);
        CHECK(size_in_bytes(15) == 2);
    }

}


TEST_CASE("powers of two", "[arithmetic][powers of two]")
{
    SECTION("next power of two")
    {
        CHECK(next_power_of_two(1) == 1);
        CHECK(next_power_of_two(17) == 32);
        CHECK(next_power_of_two(256) == 256);
        CHECK(next_power_of_two(4095) == 4096);
    }

    SECTION("computing powers of two")
    {
        CHECK(two_to_the_power_of(0) == 1);
        CHECK(two_to_the_power_of(5) == 32);
        CHECK(two_to_the_power_of(10) == 1024);
    }

    SECTION("logarithm returns the index of the most significant bit")
    {
        using allocator::log2;

        CHECK(log2(1) == 0);
        CHECK(log2(2) == 1);
        CHECK(log2(32) == 5);
        CHECK(log2(1024) == 10);
        CHECK(log2(1030) == 10);
    }

}


TEST_CASE("even and odd numbers", "[arithmetic][even and odd]")
{
    for (auto i = 0u; i < 5; ++i)
    {
        CHECK(is_even(2 * i));
        CHECK(is_odd(2 * i + 1));
    }
}


TEST_CASE("index related functions", "[arithmetic][index]")
{
    SECTION("convertion to level index")
    {
        CHECK(to_level_index(0, 0) == 0);
        CHECK(to_level_index(1, 1) == 0);
        CHECK(to_level_index(2, 1) == 1);
        CHECK(to_level_index(3, 2) == 0);
        CHECK(to_level_index(6, 2) == 3);
    }

    SECTION("first index at level")
    {
        CHECK(first_index_at(0) == 0);
        CHECK(first_index_at(1) == 1);
        CHECK(first_index_at(2) == 3);
        CHECK(first_index_at(3) == 7);
    }

    SECTION("parent index")
    {
        CHECK(parent_of(0) == 0);
        CHECK(parent_of(1) == 0);
        CHECK(parent_of(2) == 0);
        CHECK(parent_of(3) == 1);
        CHECK(parent_of(6) == 2);
    }

    SECTION("right child")
    {
        CHECK(right_child_of(0) == 2);
        CHECK(right_child_of(1) == 4);
        CHECK(right_child_of(2) == 6);
        CHECK(right_child_of(3) == 8);
    }

    SECTION("buddy")
    {
        CHECK(buddy_of(1) == 2);
        CHECK(buddy_of(2) == 1);
        CHECK(buddy_of(3) == 4);
        CHECK(buddy_of(4) == 3);
        CHECK(buddy_of(5) == 6);
        CHECK(buddy_of(6) == 5);
    }

}
