#include "bitmap.hpp"

#include <utility>

#include "arithmetic.hpp"


namespace allocator
{
    BitMap::BitMap(unsigned char* bits,
                   std::size_t size,
                   bool initial_value) :
        bits(bits),
        size(size)
    {
        assert(size == 0 || bits != nullptr);
        const auto end = size_in_bytes(size);
        const auto value = initial_byte_value_for(initial_value);

        for (auto i = std::size_t(0); i < end; ++i)
        {
            bits[i] = value;
        }
    }


    BitMap::BitMap(BitMap&& source) :
        BitMap{}
    {
        swap_contents_with(source);
    }


    void BitMap::swap_contents_with(BitMap& map)
    {
        std::swap(this->bits, map.bits);
        std::swap(this->size, map.size);
    }


    BitMap& BitMap::operator=(BitMap&& rhs)
    {
        if (this != &rhs)
        {
            auto copy = std::move(rhs);
            swap_contents_with(copy);
        }

        return *this;
    }


    bool BitMap::at(std::size_t index) const
    {
        const auto pair = byte_for_bit_at(index);

        return pair.byte & pair.mask;
    }


    BitMap::Bytes
    BitMap::byte_for_bit_at(std::size_t index) const
    {
        assert(index < this->size);
        const auto pair = divided_by_bits_in_byte(index);

        return { bits[pair.quotient],
                 static_cast<unsigned char>(
                     0b10000000 >> pair.remainder
                 ) };
    }


    void BitMap::flip(std::size_t index)
    {
        auto pair = byte_for_bit_at(index);

        pair.byte ^= pair.mask;
    }

}
