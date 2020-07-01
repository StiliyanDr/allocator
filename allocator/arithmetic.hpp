#ifndef __ARITHMETIC_HEADER_INCLUDED__
#define __ARITHMETIC_HEADER_INCLUDED__

#include <assert.h>
#include <cstddef>


namespace allocator
{
    struct DivisionResult
    {
        std::size_t quotient;
        std::size_t remainder;
    };


    std::size_t size_in_bytes(std::size_t size_in_bits);


    std::size_t blocks_fitting(std::size_t units,
                               std::size_t block_size);


    DivisionResult divide(std::size_t x, std::size_t y);


    std::size_t next_power_of_two(std::size_t x);


    std::size_t log2(std::size_t x);


    std::size_t to_level_index(std::size_t index,
                               std::size_t level);


    inline std::size_t two_to_the_power_of(std::size_t n)
    {
        return 1 << n;
    }


    inline std::size_t first_index_at(std::size_t level)
    {
        return two_to_the_power_of(level) - 1;
    }


    inline std::size_t parent_of(std::size_t index)
    {
        assert(index > 0);
        return (index - 1) / 2;
    }


    inline std::size_t right_child_of(std::size_t index)
    {
        return 2 * index + 2;
    }


    std::size_t buddy_of(std::size_t index);


    inline bool is_even(std::size_t x)
    {
        return x % 2 == 0;
    }


    inline bool is_odd(std::size_t x)
    {
        return !is_even(x);
    }

}

#endif // !__ARITHMETIC_HEADER_INCLUDED__