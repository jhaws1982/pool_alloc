#include <cstddef>
#include <cstdint>
#include "BlockAllocator.hpp"

/**
 * @class MemoryPool MemoryPool
 *
 * Define a memory pool with a set of block sizes appropriate for the
 * application.
 *
 * Assumptions:
 *  - Allocation of more than a single block is not desired, thus ensuring
 *    blocks of contiguous memory are available is not necessary. [simplifying]
 *  - All allocations are equally possible, thus dividing heap into equal
 *    segments and breaking those segments into blocks. [simplyfying]
 *  - All allocation sizes must be a power of 2, as well as the number of block
 *    sizes to allocate to ensure no wasted memory.
 */
class MemoryPool
{
public:
  /**
   * @brief MemoryPool is a singleton class - must use getPool() to obtain
   * instance for use in application code. This ensures that only one instance
   * of the memory pool ever exists per application.
   *
   * @return Reference to singleton instance of MemoryPool
   */
  static MemoryPool &getPool(const size_t *blockSize, const size_t nSizes);

  /**
   * @brief Return whether pool is initialized and ready to allocate memory
   *
   * @return true Pool is initialized and ready for use
   * @return false Pool has not yet been initialized
   */
  bool isInitialized() { return m_initialized; }

  /**
   * @brief Allocate blocks of at least n bytes from pool
   *
   * @param n Number of bytes to allocate
   * @return void* Pointer to the allocated memory block, or null on failure
   */
  void *allocate(size_t n);

  /**
   * @brief Release memory allocation pointed to by pointer p back to pool
   *
   * @param[in] p Pointer to memory block to release
   */
  void release(void *p);

private:
  // Private ctor for proper singleton behavior
  MemoryPool(const size_t *blockSize, const size_t nSizes);

  // Delete the default ctor, assignment operator, and copy ctor for singleton
  MemoryPool() = delete;
  MemoryPool &operator=(const MemoryPool &other) = delete;
  MemoryPool(const MemoryPool &other) = delete;

  // Helper routines for pool initialization
  bool scrubBlockSizes(const size_t *blockSize, const size_t nSizes);
  bool isPower2(size_t val);
  void sortArray(size_t *array, const size_t nElements);

  /** @brief Total number of bytes in the memory pool */
  static constexpr uint32_t POOLSIZE_BYTES = 65536;
  /** @brief Maximum number of block pools available for initialization/use */
  static constexpr uint8_t BLKCNT_MAX = 16;

  /** @brief Statically allocated heap used as pool memory */
  static uint8_t s_pool_heap[POOLSIZE_BYTES];
  /** @brief Array of allocators available for use */
  BlockAllocator m_allocator[BLKCNT_MAX];
  /** @brief Flag denoting whether pool has been initialized or not */
  bool m_initialized{false};
  /** @brief Size of each pool (used to find allocator to use for release) */
  size_t m_poolSize{0};
  /** @brief Number of pool allocators initialized */
  size_t m_nAllocators{0};
};