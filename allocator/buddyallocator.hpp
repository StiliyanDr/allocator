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
        static PtrValueType value_of_pointer(void* p)
        {
            return reinterpret_cast<PtrValueType>(p);
        }

        static void* as_pointer(PtrValueType p)
        {
            return reinterpret_cast<void*>(p);
        }

    private:
        void swap_contents_with(BuddyAllocator& other);

        void clear()
        {
            free_lists = nullptr;
        }

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