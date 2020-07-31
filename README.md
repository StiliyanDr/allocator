# allocator
A C++14 implementation of the buddy memory allocation algorithm.
  
## Main features
The algorithm is encapsulated in an object which is constructed with a pointer to the memory block to be managed and its size.
  
Allocation and deallocation take time O(logN), where N is the first power of two greater-than or equal to the managed block's size.
  
In terms of memory consumption for bookkeeping, no more than 5-6% of the memory block is used for big enough blocks, say bigger than 1KB. For large blocks, say at least 4KB, the memory consumption becomes insignificantly small - around 1-2%. The algorithm is not well suited to small allocations so the second figure is more indicative.
  
Move operations are also supported. The object's behaviour is illustrated in its [unit tests](https://github.com/StiliyanDr/allocator/blob/master/unit_tests/buddyallocatortests.cpp).
  
## Documentation
Documentation, notes on algorithm design, complexity and correctness can be found at the project's [wiki](https://github.com/StiliyanDr/allocator/wiki).
  
## Testing
The project's unit tests are written with [Catch2](https://github.com/catchorg/Catch2) and can be found [here](https://github.com/StiliyanDr/allocator/tree/master/unit_tests).
  
## References
[Allocation Adventures 3: The Buddy Allocator](http://bitsquid.blogspot.com/2015/08/allocation-adventures-3-buddy-allocator.html)  
[The Art of Computer Programming, Vol. 1](https://en.wikipedia.org/wiki/The_Art_of_Computer_Programming), Donald Knuth
