#include "catch.hpp"

#include "freelist.hpp"

using namespace allocator;


constexpr auto BLOCK_SIZE = 128u;
constexpr auto BLOCKS_COUNT = 4u;
alignas(std::max_align_t) static char memory[
    BLOCKS_COUNT * BLOCK_SIZE
];


void* block_at(std::size_t index)
{
    assert(index < BLOCKS_COUNT);
    return memory + index * BLOCK_SIZE;
}


FreeList create_list_with_first_n_blocks(std::size_t n)
{
    assert(n <= BLOCKS_COUNT);
    auto result = FreeList{};

    for (auto i = 0u; i < n; ++i)
    {
        result.insert(block_at(i));
    }

    return result;
}


TEST_CASE("FreeList special methods", "[free list][special methods]")
{
    auto empty_list = FreeList{};
    auto non_empty_list = FreeList{};
    non_empty_list.insert(block_at(0));

    SECTION("default ctor creates empty list")
    {
        CHECK(empty_list.is_empty());
        CHECK(empty_list.get_size() == 0);
    }

    SECTION("move ctor")
    {
        SECTION("from empty list")
        {
            const auto list = std::move(empty_list);

            CHECK(list.is_empty());
            CHECK(empty_list.is_empty());
        }

        SECTION("from non-empty list")
        {
            const auto list = std::move(non_empty_list);

            CHECK_FALSE(list.is_empty());
            CHECK(non_empty_list.is_empty());
        }

    }

    SECTION("move assignment")
    {
        SECTION("empty to empty")
        {
            auto lhs = FreeList{};

            lhs = std::move(empty_list);

            CHECK(lhs.is_empty());
            CHECK(empty_list.is_empty());
        }

        SECTION("empty to non-empty")
        {
            non_empty_list = std::move(empty_list);

            CHECK(non_empty_list.is_empty());
            CHECK(empty_list.is_empty());
        }

        SECTION("non-empty to empty")
        {
            empty_list = std::move(non_empty_list);

            CHECK_FALSE(empty_list.is_empty());
            CHECK(non_empty_list.is_empty());
        }

        SECTION("non-empty to non-empty")
        {
            auto lhs = FreeList{};
            auto rhs = FreeList{};
            lhs.insert(block_at(1));
            rhs.insert(block_at(2));

            lhs = std::move(rhs);

            REQUIRE(rhs.is_empty());
            REQUIRE_FALSE(lhs.is_empty());
            REQUIRE(lhs.extract() == block_at(2));
        }

    }

}


TEST_CASE("FreeList manipulation",
          "[free list][insertion][extraction]")
{
    SECTION("insertion and extraction happen at the front")
    {
        auto list = FreeList{};

        for (auto i = 0u; i < BLOCKS_COUNT; ++i)
        {
            list.insert(block_at(i));
        }

        for (auto i = int(BLOCKS_COUNT - 1); i >= 0; --i)
        {
            REQUIRE(list.extract() == block_at(i));
        }
    }

    SECTION("removal")
    {
        SECTION("only element")
        {
            auto singleton = create_list_with_first_n_blocks(1);

            singleton.remove(block_at(0));

            CHECK(singleton.is_empty());
        }

        SECTION("end elements")
        {
            auto list = create_list_with_first_n_blocks(3);

            list.remove(block_at(0));
            list.remove(block_at(2));

            REQUIRE_FALSE(list.is_empty());
            REQUIRE(list.get_size() == 1);
            REQUIRE(list.extract() == block_at(1));
        }

        SECTION("internal element")
        {
            auto list = create_list_with_first_n_blocks(3);

            list.remove(block_at(1));

            REQUIRE(list.get_size() == 2);
            REQUIRE(list.extract() == block_at(2));
            REQUIRE(list.extract() == block_at(0));
            REQUIRE(list.is_empty());
        }

    }

}


TEST_CASE("FreeList size related methods", "[free list][size]")
{
    auto list = create_list_with_first_n_blocks(BLOCKS_COUNT);

    for (auto i = BLOCKS_COUNT; i >= 1; --i)
    {
        REQUIRE_FALSE(list.is_empty());
        REQUIRE(list.get_size() == i);
        list.extract();
    }

    REQUIRE(list.is_empty());
    REQUIRE(list.get_size() == 0);
}
