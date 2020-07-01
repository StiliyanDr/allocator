#include "arithmetic.hpp"


namespace allocator
{
    constexpr std::size_t BITS_IN_BYTE = 8;


    constexpr std::size_t SIZE_T_SIZE_IN_BITS =
        BITS_IN_BYTE * sizeof(std::size_t);


    std::size_t size_in_bytes(std::size_t size_in_bits)
    {
        return blocks_fitting(size_in_bits, BITS_IN_BYTE);
    }


    std::size_t blocks_fitting(std::size_t units,
                               std::size_t block_size)
    {
        const auto pair = divide(units, block_size);

        return pair.quotient + (pair.remainder != 0 ? 1 : 0);
    }


    DivisionResult divide(std::size_t x, std::size_t y)
    {
        assert(y != 0);
        return { x / y, x % y };
    }


    std::size_t next_power_of_two(std::size_t x)
    {
        assert(x > 0);
        --x;

        for (auto i = std::size_t(1);
             i < SIZE_T_SIZE_IN_BITS;
             i <<= 1)
        {
            x |= x >> i;
        }

        return x + 1;
    }


    std::size_t log2(std::size_t x)
    {
        assert(x > 0);
        auto result = std::size_t(0);

        for (auto i = std::size_t(SIZE_T_SIZE_IN_BITS / 2);
             i > 0;
             i >>= 1)
        {
            if (x >= (std::size_t(1) << i))
            {
                result += i;
                x >>= i;
            }
        }

        return result;
    }


    std::size_t to_level_index(std::size_t index, std::size_t level)
    {
        return index - first_index_at(level);
    }


    std::size_t buddy_of(std::size_t index)
    {
        assert(index > 0);
        return index + is_even(index) ? -1 : 1;
    }

}
