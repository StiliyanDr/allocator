#include "buddyallocator.hpp"

#include <assert.h>
#include <memory>
#include <stdexcept>
#include <utility>

#include "insufficientmemory.hpp"


namespace allocator
{
    BuddyAllocator::BuddyAllocator() :
        free_lists(nullptr)
    {
    }


    BuddyAllocator::BuddyAllocator(void* memory, std::size_t size)
    {
        verify_pointer_is_not_null(memory);
        const auto memory_descriptor =
            set_logical_start_size_and_levels_count(memory, size);
        const auto free_memory_start =
            create_data_structures(memory_descriptor);
        initialise_data_structures(free_memory_start);
    }


    void BuddyAllocator::verify_pointer_is_not_null(void* memory)
    {
        if (memory == nullptr)
        {
            throw std::invalid_argument{ "Expected a pointer to memory!" };
        }
    }


    BuddyAllocator::MemoryDescriptor
    BuddyAllocator::set_logical_start_size_and_levels_count(
        void* memory,
        std::size_t size
    )
    {
        const auto aligned_memory = first_aligned_address_within(memory, size);
        auto actual_size = std::size_t(size - subtract(aligned_memory, memory));
        const auto wasted_bytes_at_end = actual_size % LEAF_SIZE;
        actual_size -= wasted_bytes_at_end;

        set_size(actual_size);
        set_start(aligned_memory, actual_size);
        set_levels_count();

        return { aligned_memory, actual_size, wasted_bytes_at_end };
    }


    void* BuddyAllocator::first_aligned_address_within(void* memory,
                                                       std::size_t size)
    {
        const auto result = std::align(ALIGNMENT_REQUIREMENT,
                                       LEAF_SIZE,
                                       memory,
                                       size);

        if (result != nullptr)
        {
            return result;
        }
        else
        {
            throw InsufficientMemory{};
        }
    }


    std::size_t BuddyAllocator::subtract(void* a, void* b)
    {
        assert(a >= b);

        return value_of_pointer(a) - value_of_pointer(b);
    }


    void BuddyAllocator::set_size(std::size_t actual_size)
    {
        assert(actual_size >= LEAF_SIZE);
        size = next_power_of_two(actual_size);
    }


    void BuddyAllocator::set_start(void* actual_start,
                                   std::size_t actual_size)
    {
        const auto logical_memory_size = std::size_t(size - actual_size);
        start = value_of_pointer(actual_start) - logical_memory_size;
    }


    void BuddyAllocator::set_levels_count()
    {
        levels_count = log2(size / LEAF_SIZE) + 1;

        if (levels_count < MIN_LEVELS_COUNT)
        {
            throw InsufficientMemory{};
        }
    }


    void* BuddyAllocator::create_data_structures(MemoryDescriptor memory)
    {
        memory = create_free_lists(memory);

        return create_maps(memory);
    }


    BuddyAllocator::MemoryDescriptor
    BuddyAllocator::create_free_lists(const MemoryDescriptor& memory)
    {
        const auto size_per_list = compute_size_per_list(memory);
        const auto size_for_lists = levels_count * size_per_list;

        if (size_for_lists < memory.size)
        {
            free_lists = new (memory.start) FreeList[levels_count];

            return { add_to(memory.start, size_for_lists),
                     memory.size - size_for_lists,
                     memory.wasted_bytes_at_end };
        }
        else
        {
            throw InsufficientMemory{};
        }
    }


    std::size_t BuddyAllocator::compute_size_per_list(
        const MemoryDescriptor& memory
    )
    {
        const auto size_of_list = sizeof(FreeList);

        assert(size_of_list < memory.size);
        auto first_list_end = add_to(memory.start, size_of_list);
        auto available_space = std::size_t(memory.size - size_of_list);
        const auto second_list = std::align(alignof(FreeList),
                                            size_of_list,
                                            first_list_end,
                                            available_space);
        assert(second_list != nullptr);

        return subtract(second_list, memory.start);
    }


    void* BuddyAllocator::create_maps(const MemoryDescriptor& memory)
    {
        const auto bit_map_size = two_to_the_power_of(levels_count - 1);
        const auto bit_map_size_in_bytes = size_in_bytes(bit_map_size);
        const auto maps_start = determine_maps_storage(
            2 * bit_map_size_in_bytes,
            memory
        );

        split_map = BitMap(maps_start, bit_map_size - 1);
        free_map = BitMap(maps_start + bit_map_size_in_bytes, bit_map_size);
        free_map.flip(0);

        return add_to(
            memory.start,
            maps_start == memory.start ?
            2 * bit_map_size_in_bytes : 0
        );
    }


    unsigned char* BuddyAllocator::determine_maps_storage(
        std::size_t maps_size,
        const MemoryDescriptor& memory
    )
    {
        auto result = static_cast<void*>(nullptr);

        if (maps_size <= memory.wasted_bytes_at_end)
        {
            result = add_to(memory.start, memory.size);
        }
        else if (maps_size < memory.size)
        {
            result = memory.start;
        }
        else
        {
            throw InsufficientMemory{};
        }

        return reinterpret_cast<unsigned char*>(result);
    }


    void BuddyAllocator::initialise_data_structures(
        void* free_memory_start
    )
    {
        const auto preallocated_size =
            value_of_pointer(free_memory_start) - start;
        const auto first_leaf = first_index_at(levels_count - 1);
        const auto last_leaf_to_allocate =
            first_leaf + size_in_leaves(preallocated_size) - 1;
        preallocate_leaves_until(last_leaf_to_allocate);
    }


    void BuddyAllocator::preallocate_leaves_until(std::size_t leaf)
    {
        mark_blocks_as_allocated_until(leaf, levels_count - 1);

        preallocate_leaves_parents_until(
            parent_of(leaf),
            leaf
        );
    }


    void BuddyAllocator::mark_blocks_as_allocated_until(std::size_t index,
                                                        std::size_t level)
    {
        if (is_even(to_level_index(index, level)))
        {
            flip_free_map_at(index);
        }
    }


    void BuddyAllocator::flip_free_map_at(std::size_t index)
    {
        free_map.flip(free_map_index_for(index));
    }


    bool BuddyAllocator::free_map_at(std::size_t index) const
    {
        return free_map.at(free_map_index_for(index));
    }


    void BuddyAllocator::preallocate_leaves_parents_until(
        std::size_t index,
        std::size_t last_preallocated_leaf
    )
    {
        auto level = int(levels_count - 2);
        auto last_block_to_allocate = index;
        auto last_allocated_child = last_preallocated_leaf;

        while (level >= 0)
        {
            mark_blocks_as_split_until(last_block_to_allocate, level);
            mark_blocks_as_allocated_until(last_block_to_allocate, level);
            insert_free_blocks_at(level + 1,
                                  last_allocated_child,
                                  last_block_to_allocate);

            --level;
            last_allocated_child = last_block_to_allocate;
            last_block_to_allocate = parent_of(last_block_to_allocate);
        }
    }


    void BuddyAllocator::mark_blocks_as_split_until(std::size_t index,
                                                    std::size_t level)
    {
        assert(level < levels_count - 1);

        for (auto i = first_index_at(level);
             i <= index;
             ++i)
        {
            split_map.flip(i);
        }
    }


    void BuddyAllocator::insert_free_blocks_at(std::size_t level,
                                               std::size_t last_allocated_block,
                                               std::size_t last_allocated_parent)
    {
        assert(level < levels_count);
        const auto right_child = right_child_of(last_allocated_parent);

        if (right_child > last_allocated_block)
        {
            free_lists[level].insert(
                to_address(right_child, level)
            );
        }
    }


    void* BuddyAllocator::to_address(std::size_t index,
                                     std::size_t level) const
    {
        const auto preceding_blocks_count =
            index - first_index_at(level);

        return as_pointer(start +
                          preceding_blocks_count * size_at(level));
    }


    BuddyAllocator::BuddyAllocator(BuddyAllocator&& source) :
        start(source.start),
        size(source.size),
        levels_count(source.levels_count),
        free_lists(source.free_lists),
        split_map(std::move(source.split_map)),
        free_map(std::move(source.free_map))
    {
        source.clear();
    }


    BuddyAllocator& BuddyAllocator::operator=(BuddyAllocator&& rhs)
    {
        if (this != &rhs)
        {
            auto copy = std::move(rhs);
            swap_contents_with(copy);
        }

        return *this;
    }


    void BuddyAllocator::swap_contents_with(BuddyAllocator& other)
    {
        std::swap(this->start, other.start);
        std::swap(this->size, other.size);
        std::swap(this->levels_count, other.levels_count);
        std::swap(this->free_lists, other.free_lists);
        std::swap(this->split_map, other.split_map);
        std::swap(this->free_map, other.free_map);
    }


    void* BuddyAllocator::allocate(std::size_t size)
    {
        auto block = static_cast<void*>(nullptr);

        if (manages_memory() && size != 0)
        {
            const auto level = level_for_block_with(size);

            if (level != -1)
            {
                assert(level >= 0);
                assert(level < levels_count);
                block = allocate_block_at(level);
            }
        }

        return block;
    }


    int BuddyAllocator::level_for_block_with(std::size_t size) const
    {
        assert(size > 0);

        if (size <= LEAF_SIZE)
        {
            return levels_count - 1;
        }
        else if (size <= this->size)
        {
            return level_for_block_with_power_of_two_size(
                next_power_of_two(size)
            );
        }
        else
        {
            return -1;
        }
    }


    void* BuddyAllocator::allocate_block_at(std::size_t level)
    {
        auto& free_blocks = free_lists[level];

        if (free_blocks.is_empty())
        {
            if (level == 0)
            {
                return nullptr;
            }
            const auto block = allocate_block_at(level - 1);

            if (block != nullptr)
            {
                split_map.flip(index_of(block, level - 1));
                insert_in(free_blocks, block, add_to(block, size_at(level)));
            }
            else
            {
                return nullptr;
            }
        }

        const auto result = free_blocks.extract();
        flip_free_map_at(index_of(result, level));

        return result;
    }


    std::size_t BuddyAllocator::index_of(void* ptr,
                                         std::size_t level) const
    {
        return first_index_at(level) + index_at(level, ptr);
    }


    std::size_t BuddyAllocator::index_at(std::size_t level,
                                         void* ptr) const
    {
        return (value_of_pointer(ptr) - start) / size_at(level);
    }


    void BuddyAllocator::deallocate(void* block)
    {
        if (manages_memory() && block != nullptr)
        {
            free(block, level_of(block));
        }
    }


    std::size_t BuddyAllocator::level_of(void* p) const
    {
        auto level = levels_count - 1;
        auto index = index_of(p, level);

        while (level > 0)
        {
            const auto parent = parent_of(index);

            if (split_map.at(parent))
            {
                break;
            }
            else
            {
                --level;
                index = parent;
            }
        }

        return level;
    }


    void BuddyAllocator::free(void* block, std::size_t level)
    {
        assert(block != nullptr);
        assert(level < levels_count);

        free(block, level, index_of(block, level));
    }


    void BuddyAllocator::free(void* block,
                              std::size_t level,
                              std::size_t index)
    {
        auto& free_blocks = free_lists[level];

        if (free_map_at(index))
        {
            assert(level > 0);
            const auto buddy_level_index =
                to_level_index(buddy_of(index), level);
            const auto buddy =
                as_pointer(start + buddy_level_index * size_at(level));
            free_blocks.remove(buddy);
            const auto parent = parent_of(index);
            split_map.flip(parent);
            free(to_address(parent, level - 1), level - 1, parent);
        }
        else
        {
            free_blocks.insert(block);
        }

        flip_free_map_at(index);
    }


    void BuddyAllocator::deallocate(void* block, std::size_t size)
    {
        if (manages_memory() && block != nullptr)
        {
            const auto level =
                level_for_block_with(size);
            assert(level >= 0);
            free(block, level);
        }
    }

}