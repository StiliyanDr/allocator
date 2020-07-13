#ifndef __INSUFFICIENT_MEMORY_HEADER_INCLUDED__
#define __INSUFFICIENT_MEMORY_HEADER_INCLUDED__

#include <stdexcept>


namespace allocator
{
    class InsufficientMemory : public std::runtime_error
    {
    public:
        InsufficientMemory() :
            InsufficientMemory{ "Insufficient memory!" }
        {
        }

        explicit InsufficientMemory(const char* message) :
            std::runtime_error{ message }
        {
        }

    };

}

#endif // __INSUFFICIENT_MEMORY_HEADER_INCLUDED__