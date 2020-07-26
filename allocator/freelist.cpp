#include "freelist.hpp"

#include <utility>


namespace allocator
{
    FreeList::FreeList(FreeList&& source) :
        FreeList{}
    {
        swap_contents_with(source);
    }


    void FreeList::swap_contents_with(FreeList& list)
    {
        std::swap(this->first, list.first);
    }


    FreeList& FreeList::operator=(FreeList&& rhs)
    {
        if (this != &rhs)
        {
            auto copy = std::move(rhs);
            swap_contents_with(copy);
        }

        return *this;
    }


    void FreeList::insert(void* block)
    {
        validate_alignment_of(block);
        previous_to(block) = value_of_pointer(nullptr);
        next_to(block) = value_of_pointer(first);

        if (!is_empty())
        {
            previous_to(first) = value_of_pointer(block);
        }

        first = block;
    }


    void FreeList::remove(void* block)
    {
        assert(!is_empty());
        validate_alignment_of(block);
        const auto previous = as_pointer(previous_to(block));

        if (previous != nullptr)
        {
            next_to(previous) = next_to(block);
        }
        else
        {
            first = as_pointer(next_to(block));
        }

        const auto next = as_pointer(next_to(block));

        if (next != nullptr)
        {
            previous_to(next) = previous_to(block);
        }
    }


    void* FreeList::extract()
    {
        assert(!is_empty());
        const auto result = first;
        remove(first);

        return result;
    }


    std::size_t FreeList::get_size() const
    {
        auto current = first;
        auto size = std::size_t(0);

        while (current != nullptr)
        {
            ++size;
            current = as_pointer(next_to(current));
        }

        return size;
    }

}
