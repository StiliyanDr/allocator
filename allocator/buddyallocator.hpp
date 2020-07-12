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

    public:
        BuddyAllocator(void* memory, std::size_t size);
        BuddyAllocator(BuddyAllocator&&);
        BuddyAllocator& operator=(BuddyAllocator&&);

        void* allocate(std::size_t size);
        void deallocate(void* block);
        void deallocate(void* block, std::size_t size);

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