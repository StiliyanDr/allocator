#include <stdexcept>
#include <unordered_set>

#include "catch.hpp"

#include "buddyallocator.hpp"
#include "insufficientmemory.hpp"

namespace alc = allocator;


constexpr auto MIN_SIZE = 256u;
constexpr auto SIZE = 4096u;
alignas(std::max_align_t) static char memory[SIZE];


struct AllocationsResult
{
    std::unordered_set<void*> blocks;
    bool an_address_was_duplicated = false;
};


AllocationsResult
allocate_memory_in_small_blocks(alc::BuddyAllocator& allocator)
{
    auto result = AllocationsResult{};

    while (true)
    {
        const auto block = allocator.allocate(1);

        if (block != nullptr)
        {
            const auto p = result.blocks.insert(block);

            if (!p.second)
            {
                result.an_address_was_duplicated = true;
            }
        }
        else
        {
            break;
        }
    }

    return result;
}


void deallocate(const std::unordered_set<void*>& blocks,
                alc::BuddyAllocator& allocator)
{
    for (auto block : blocks)
    {
        allocator.deallocate(block);
    }
}


std::uintptr_t value_of(void* ptr)
{
    return reinterpret_cast<std::uintptr_t>(ptr);
}


bool is_within_memory_block(void* ptr,
                            void* block = memory,
                            std::size_t size = SIZE)
{
    assert(size > 0);

    const auto start = value_of(block);
    const auto end = start + (size - 1);
    const auto p = value_of(ptr);

    return start <= p && p <= end;
}


bool is_correctly_aligned(void* ptr)
{
    return value_of(ptr) % alignof(std::max_align_t) == 0;
}


bool is_valid_pointer(void* ptr,
                      void* block = memory,
                      std::size_t size = SIZE)
{
    return is_within_memory_block(ptr, block, size) &&
           is_correctly_aligned(ptr);
}


bool are_valid_pointers(const std::unordered_set<void*>& blocks,
                        void* memory_block = memory,
                        std::size_t size = SIZE)
{
    return std::all_of(
        blocks.begin(),
        blocks.end(),
        [memory_block, size](auto block)
        {
            return is_valid_pointer(block, memory_block, size);
        }
    );
}


TEST_CASE("BuddyAllocator special members",
          "[buddy allocator][special members]")
{
    SECTION("default ctor creates an object that manages no memory")
    {
        const auto allocator = alc::BuddyAllocator{};

        REQUIRE_FALSE(allocator.manages_memory());
    }

    SECTION("ctor from memory")
    {
        SECTION("exception is thrown for nullptr")
        {
            REQUIRE_THROWS_AS(alc::BuddyAllocator(nullptr, 0),
                              std::invalid_argument);
        }

        SECTION("exception is thrown when memory is insufficient after alignment")
        {
            REQUIRE_THROWS_AS(alc::BuddyAllocator(memory, 0),
                              alc::InsufficientMemory);
            REQUIRE_THROWS_AS(alc::BuddyAllocator(memory + 1, MIN_SIZE),
                              alc::InsufficientMemory);
        }

        SECTION("no exception is thrown when memory is enough")
        {
            REQUIRE_NOTHROW(alc::BuddyAllocator(memory, MIN_SIZE));
        }

    }

    SECTION("move ctor leaves source with no memory to manage")
    {
        SECTION("from empty allocator")
        {
            auto empty = alc::BuddyAllocator{};

            const auto allocator = std::move(empty);

            CHECK_FALSE(empty.manages_memory());
            CHECK_FALSE(allocator.manages_memory());
        }

        SECTION("from non-empty allocator")
        {
            auto source = alc::BuddyAllocator(memory, SIZE);

            auto allocator = std::move(source);

            REQUIRE_FALSE(source.manages_memory());
            REQUIRE(allocator.manages_memory());

            SECTION("moved-into allocator is fully functioning")
            {
                const auto p = allocator.allocate(1);

                REQUIRE(is_valid_pointer(p));

                allocator.deallocate(p);
            }
        }

    }

    SECTION("move assignment")
    {
        SECTION("emtpy to emtpy")
        {
            auto lhs = alc::BuddyAllocator{};
            auto rhs = alc::BuddyAllocator{};

            lhs = std::move(rhs);

            CHECK_FALSE(lhs.manages_memory());
            CHECK_FALSE(rhs.manages_memory());
        }

        SECTION("empty to non-empty")
        {
            auto lhs = alc::BuddyAllocator(memory, SIZE);
            auto rhs = alc::BuddyAllocator{};

            lhs = std::move(rhs);

            CHECK_FALSE(lhs.manages_memory());
            CHECK_FALSE(rhs.manages_memory());
        }

        SECTION("non-empty to empty")
        {
            auto lhs = alc::BuddyAllocator{};
            auto rhs = alc::BuddyAllocator(memory, SIZE);

            lhs = std::move(rhs);

            CHECK_FALSE(rhs.manages_memory());
            CHECK(lhs.manages_memory());
        }

        SECTION("non-empty to non-empty")
        {
            const auto size = SIZE / 2;
            auto lhs = alc::BuddyAllocator(memory + size, size);
            auto rhs = alc::BuddyAllocator(memory, size);

            lhs = std::move(rhs);

            REQUIRE_FALSE(rhs.manages_memory());
            REQUIRE(lhs.manages_memory());

            SECTION("moved into allocator is fully functioning")
            {
                const auto p = lhs.allocate(1);

                REQUIRE(is_valid_pointer(p, memory, size));

                lhs.deallocate(p);
            }
        }

    }

}


TEST_CASE("BuddyAllocator allocation and deallocation",
          "[buddy allocator][allocation][deallocation]")
{
    SECTION("empty allocators have dummy behaviour")
    {
        auto allocator = alc::BuddyAllocator{};

        REQUIRE_FALSE(allocator.manages_memory());
        REQUIRE(allocator.allocate(1) == nullptr);
        REQUIRE_NOTHROW(allocator.deallocate(nullptr));
        REQUIRE_NOTHROW(allocator.deallocate(nullptr, 0));
    }

    SECTION("edge cases")
    {
        auto allocator = alc::BuddyAllocator(memory, SIZE);

        REQUIRE(allocator.allocate(0) == nullptr);
        REQUIRE_NOTHROW(allocator.deallocate(nullptr));
        REQUIRE_NOTHROW(allocator.deallocate(nullptr, 0));
    }

    SECTION("many allocations and deallocations")
    {
        const auto memory_block = memory + 1;
        const auto size = SIZE - 1;
        auto allocator = alc::BuddyAllocator(memory_block, size);

        const auto initial_allocation = \
            allocate_memory_in_small_blocks(allocator);
        REQUIRE_FALSE(initial_allocation.an_address_was_duplicated);
        REQUIRE(are_valid_pointers(initial_allocation.blocks,
                                   memory_block,
                                   size));
        deallocate(initial_allocation.blocks, allocator);

        const auto big_block = allocator.allocate(size / 2);
        REQUIRE(big_block != nullptr);
        allocator.deallocate(big_block);

        const auto second_allocation =
            allocate_memory_in_small_blocks(allocator);
        REQUIRE(initial_allocation.blocks == second_allocation.blocks);
        deallocate(second_allocation.blocks, allocator);
    }

}
