#ifndef __BUDDY_ALLOCATOR_HEADER_INCLUDED__
#define __BUDDY_ALLOCATOR_HEADER_INCLUDED__

#include <cstddef>
#include <cstdint>

#include "bitmap.hpp"
#include "freelist.hpp"


namespace allocator
{
    class BuddyAllocator
    {
        using PtrValueType = intptr_t;

        struct MemoryDescriptor
        {
            void* start;
            std::size_t size;
            std::size_t wasted_bytes_at_end;
        };

    public:
        BuddyAllocator();
        BuddyAllocator(void* memory, std::size_t size);
        BuddyAllocator(BuddyAllocator&& source);
        BuddyAllocator& operator=(BuddyAllocator&& rhs);

        void* allocate(std::size_t size);
        void deallocate(void* block);
        void deallocate(void* block, std::size_t size);

        bool manages_memory() const
        {
            return free_lists != nullptr;
        }

    private:
        static void verify_pointer_is_not_null(void* memory);
        static void* first_aligned_address_within(void* memory,
                                                  std::size_t size);
        static std::size_t compute_size_per_list(
            const MemoryDescriptor& memory
        );
        static unsigned char* determine_maps_storage(
            std::size_t maps_size,
            const MemoryDescriptor& memory
        );
        static std::size_t subtract(void* a, void* b);

        static void* add_to(void* p, std::size_t offset)
        {
            return reinterpret_cast<char*>(p) + offset;
        }

        static PtrValueType value_of_pointer(void* p)
        {
            return reinterpret_cast<PtrValueType>(p);
        }

        static void* as_pointer(PtrValueType p)
        {
            return reinterpret_cast<void*>(p);
        }

        static std::size_t size_in_leaves(std::size_t size_in_bytes)
        {
            return blocks_fitting(size_in_bytes, LEAF_SIZE);
        }

        static std::size_t free_map_index_for(std::size_t index)
        {
            index += index % 2;

            return index / 2;
        }

    private:
        MemoryDescriptor set_logical_start_size_and_levels_count(
            void* memory,
            std::size_t size
        );
        void set_size(std::size_t actual_size);
        void set_start(void* actual_start, std::size_t actual_size);
        void set_levels_count();
        void* create_data_structures(MemoryDescriptor memory);
        MemoryDescriptor create_free_lists(const MemoryDescriptor& memory);
        void* create_maps(const MemoryDescriptor& memory);
        void initialise_data_structures(void* free_memory_start);
        void preallocate_leaves_until(std::size_t leaf);
        void mark_blocks_as_allocated_until(std::size_t index,
                                            std::size_t level);
        void mark_blocks_as_split_until(std::size_t index, std::size_t level);
        void preallocate_leaves_parents_until(std::size_t index,
                                              std::size_t last_preallocated_leaf);
        void insert_free_blocks_at(std::size_t level,
                                   std::size_t last_allocated_block,
                                   std::size_t last_allocated_parent);
        void* to_address(std::size_t index, std::size_t level) const;
        void* allocate_block_at(std::size_t level);
        int level_for_block_with(std::size_t size) const;
        std::size_t index_at(std::size_t level, void* ptr) const;
        std::size_t index_of(void* ptr, std::size_t level) const;
        void flip_free_map_at(std::size_t index);
        bool free_map_at(std::size_t index) const;
        void swap_contents_with(BuddyAllocator& other);

        std::size_t level_for_block_with_power_of_two_size(
            std::size_t s
        ) const
        {
            return log2(size / s);
        }

        std::size_t size_at(std::size_t level) const
        {
            return size / two_to_the_power_of(level);
        }

        void clear()
        {
            free_lists = nullptr;
        }

    private:
        static const std::size_t LEAF_SIZE = 128;
        static const std::size_t MIN_LEVELS_COUNT = 2;
        static const std::size_t ALIGNMENT_REQUIREMENT =
            alignof(std::max_align_t);

    private:
        PtrValueType start;
        std::size_t size;
        std::size_t levels_count;
        FreeList* free_lists;
        BitMap split_map;
        BitMap free_map;
    };

}

#endif // __BUDDY_ALLOCATOR_HEADER_INCLUDED__