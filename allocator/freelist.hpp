#ifndef __FREE_LIST_HEADER_INCLUDED__
#define __FREE_LIST_HEADER_INCLUDED__

#include <cstddef>
#include <cstdint>


namespace allocator
{
    class FreeList
    {
        using PtrValueType = std::uintptr_t;

    public:
        FreeList() :
            first(nullptr)
        {
        }

        FreeList(FreeList&& source);
        FreeList& operator=(FreeList&& rhs);

        void insert(void* block);
        void* extract();
        void remove(void* block);

        std::size_t get_size() const;

        bool is_empty() const
        {
            return first == nullptr;
        }

    private:
        static PtrValueType& previous_to(void* block)
        {
            return *as_pointer_to_ptr_values(block);
        }

        static PtrValueType* as_pointer_to_ptr_values(void* block)
        {
            return reinterpret_cast<PtrValueType*>(block);
        }

        static PtrValueType& next_to(void* block)
        {
            return *(as_pointer_to_ptr_values(block) + 1);
        }

        static PtrValueType value_of_pointer(void* p)
        {
            return reinterpret_cast<PtrValueType>(p);
        }

        static void* as_pointer(PtrValueType p)
        {
            return reinterpret_cast<void*>(p);
        }

        static void validate_alignment_of(void* block);

    private:
        void swap_contents_with(FreeList& list);

    private:
        void* first;
    };


    inline void insert_in(FreeList& list,
                          void* first_block,
                          void* second_block)
    {
        list.insert(first_block);
        list.insert(second_block);
    }

}

#endif // __FREE_LIST_HEADER_INCLUDED__