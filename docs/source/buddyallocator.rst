
The BuddyAllocator class
========================

.. cpp:class:: allocator::BuddyAllocator

   Encapsulates the buddy memory allocation algorithm into an object which 
   (optionally) manages a memory block.

   Objects which manage no memory block, for example moved-from objects, 
   are said to be *empty*\ . They have dummy behaviour for (de)allocation
   requests.

   .. cpp:function:: BuddyAllocator()

      Creates an object that manages no memory.

      Complexity: O(1)

   .. cpp:function:: BuddyAllocator(void* memory, std::size_t size)

      Creates an object that manages the block pointed to by `memory`.

      When aligned to `alignof(std::max_align_t)`, the block's size must be 
      at least 256 bytes.

      :param memory: a pointer to a block of memory.
      :param size: the size of the block to be managed.

      :throws std::invalid_argument: if `memory` is a null pointer.
      :throws InsufficientMemory: if the block to be managed is not big enough, as described above.

      Complexity: O(N), where N is the first power of two ≥ `size`.

      .. note::

         The allocator is not responsible for the managed block's lifetime, 
         it assumes the block is available throughout the object's whole 
         lifetime.

   .. cpp:function:: BuddyAllocator(BuddyAllocator&& source)

      Creates an allocator by moving an existing one into it.

      :param source: the allocator to move into the newly created object.

      If `source` manages memory, this memory will be managed by the new 
      object. If it does not, the new object won't manage a memory block. 
      In both cases `source` is empty after the call.

      Complexity: O(1)

   .. cpp:function:: BuddyAllocator& operator=(BuddyAllocator&& rhs)

      Moves the allocator referred to by `rhs` into \*this.

      :param rhs: the allocator to move into \*this.

      If `rhs` manages memory, this memory will be managed by \*this. If it 
      does not, \*this won't manage a memory block. In both cases `rhs` is 
      empty after the call.

      :returns: the object being assigned to.

      Complexity: O(1)

   .. cpp:function:: ~BuddyAllocator()

      Destroys the allocator.

      Complexity: O(1)

   .. cpp:function:: bool manages_memory() const

      :returns: whether the object manages a memory block.

      Complexity: O(1)

   .. cpp:function:: void* allocate(std::size_t size)

      :param size: the size (in bytes) of the block to be allocated.

      :returns: a null pointer if the object manages no memory, 0 bytes are 
         requested or a block with the requested size can't be allocated. 
         Otherwise, a pointer to a block whose size is at least as big as 
         the requested one is returned. The pointer is aligned to 
         `alignof(std::max_align_t)`.

      Complexity: O(logN)

   .. cpp:function:: void deallocate(void* block)

      :param block: a pointer to the block to deallocate.

      If the object manages no memory or `block` is a null pointer, the 
      method does nothing. Otherwise, `block` is assumed to be the address 
      of a block allocated by the object.

      Complexity: O(logN)

   .. cpp:function:: void deallocate(void* block, std::size_t size)

      :param block: a pointer to the block to deallocate.
      :param size: the size (in bytes) of the block to deallocate.

      If the object manages no memory or `block` is a null pointer, the
      method does nothing. Otherwise, `block` is assumed to be the address 
      of a block allocated by the object and `size` is assumed to be the 
      size requested when allocating the block.

      Complexity: O(logN)
