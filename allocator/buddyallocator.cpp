#include "buddyallocator.hpp"

#include <utility>


namespace allocator
{
    BuddyAllocator::BuddyAllocator() :
        free_lists(nullptr)
    {
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

}