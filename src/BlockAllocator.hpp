#include <cstddef>
#include <cstdint>
#include <mutex>

/**
 * @class MemoryBlock MemoryBlock
 *
 * Define memory blocks using a free list for use by the allocator
 */
class MemoryBlock
{
public:
  /** @brief Free list pointer
   *
   * Handled similar to a SLUB frame pointer where the block actually contains
   * the reference to the next block in the list. The user of the block is free
   * to overwrite and once they release it back to the allocator it is simply
   * tied back into the free list.
   */
  MemoryBlock *m_next{nullptr};
};

/**
 * @class BlockAllocator BlockAllocator
 *
 * Define an allocation block (i.e. chunk of the heap) of a predetermined size
 * containing a free list of MemoryBlock objects. These objects are singly
 * allocated and deallocated by the user as necessary for their application.
 *
 * Allocation will remove the current block from the list and move the block
 * pointer to the next block in the list.  The current block is returned to the
 * user.
 *
 * Deallocation (release) will add the provided block back to the front of the
 * linked list and update the block pointer.
 */
class BlockAllocator
{
public:
  BlockAllocator(size_t blockSize, uint32_t numBytes, MemoryBlock *p);
  BlockAllocator() = default;
  ~BlockAllocator() = default;
  BlockAllocator(const BlockAllocator &ma);
  BlockAllocator &operator=(const BlockAllocator &ma);

  /**
   * @brief Remove a memory block from the chain and return the address
   *
   * @return MemoryBlock* Address of the available memory block, now unavailable
   */
  MemoryBlock *allocateBlock();

  /**
   * @brief Release a memory block back to the pool
   *
   * @param p Address of the memory block to re-add to the available chain
   * @return true Block was successfully released back to the allocator
   * @return false Block was not found to lie within the allocator page(s)
   */
  bool releaseBlock(void *p);

  /**
   * @brief Get the start address of the allocator
   *
   * Note that this does *not* return the start address of the first, currently
   * available block in the allocator; rather this returns the starting address
   * of the underlying block of memory the smaller memory blocks are contained
   * within.
   *
   * @return MemoryBlock* Address of the beginning of the allocator's memory
   * space
   */
  MemoryBlock *getStartAddress() { return m_startBlock; }

  /**
   * @brief Get the size of the blocks this allocator serves
   *
   * @return size_t Size of the blocks returned by this allocator
   */
  size_t getBlockSize() { return m_blockSize; }

  /**
   * @brief Debug routine to show number of free blocks in allocator
   */
  void showFree();

private:
  /**
   * @brief Determine if block is already in free list or not
   *
   * @param p Pointer to look for in free list
   * @return true Block is present in the list
   * @return false Block is not present in the list
   */
  bool inChain(void *p);

  /** @brief Block size for this allocator */
  size_t m_blockSize;
  /** @brief Number of blocks available to the allocator */
  size_t m_numBlocks;
  /** @brief Pointer to the next available block in the allocator */
  MemoryBlock *m_freeBlock{nullptr};
  /** @brief Pointer to the first block in the allocator */
  MemoryBlock *m_startBlock{nullptr};
  /** @brief Mutex to protect pointers during allocation/deallocation */
  std::mutex m_lock;
};