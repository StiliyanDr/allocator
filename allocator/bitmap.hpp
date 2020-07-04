#ifndef __BITMAP_HEADER_INCLUDED__
#define __BITMAP_HEADER_INCLUDED__

#include <cstddef>


namespace allocator
{
    class BitMap
    {
        struct Bytes
        {
            unsigned char& byte;
            unsigned char mask;
        };

    public:
        BitMap() :
            bits(nullptr),
            size(0)
        {
        }

        BitMap(unsigned char* bits,
               std::size_t size,
               bool initial_value = false);
        BitMap(BitMap&& source);
        BitMap& operator=(BitMap&& rhs);

        bool at(std::size_t index) const;
        void flip(std::size_t index);

        bool is_empty() const
        {
            return size == 0;
        }

        std::size_t get_size() const
        {
            return size;
        }

    private:
        static unsigned char
        initial_byte_value_for(bool initial_flag_value)
        {
            return initial_flag_value ? 0b11111111 : 0;
        }

    private:
        Bytes byte_for_bit_at(std::size_t index) const;
        void swap_contents_with(BitMap& map);

    private:
        unsigned char* bits;
        std::size_t size;
    };

}

#endif // __BITMAP_HEADER_INCLUDED__