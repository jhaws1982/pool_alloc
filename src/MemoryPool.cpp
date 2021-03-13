#include "MemoryPool.hpp"

#ifdef DEBUG
#include <iostream>
#endif

// Statically define the memory pool storage
uint8_t MemoryPool::s_pool_heap[POOLSIZE_BYTES];

MemoryPool &MemoryPool::getPool(const size_t *blockSize, const size_t nSizes)
{
  // Core of the Meyer's singleton. Static declaration here ensures this is
  // constructed *exactly* one time through the entire life of the application
  static MemoryPool s_pool(blockSize, nSizes);
  return s_pool;
}

void *MemoryPool::allocate(size_t n)
{
  MemoryBlock *block = nullptr;

  for (size_t i = 0; i < m_nAllocators; ++i)
  {
    // If the allocator block size is greater than or equal to the requested
    // number of bytes, request a block from this allocator.
    if (m_allocator[i].getBlockSize() >= n)
    {
      block = m_allocator[i].allocateBlock();

      // Need to check if the block was allocated from this pool, because if it
      // failed to allocate from the smallest pool, then we should allocate a
      // block from a larger pool
      if (block)
      {
#ifdef DEBUG
        std::cout << " ** Allocated block of size "
                  << m_allocator[i].getBlockSize() << " for " << n
                  << " bytes @ " << block << std::endl;
        m_allocator[i].showFree();
#endif
        break;
      }
    }
  }

  return block;
}

void MemoryPool::release(void *p)
{
#ifdef DEBUG
  std::cout << " ** Releasing block @ " << p << std::endl;
#endif

  // Ask each allocator to release, in turn, until block is released to the pool
  for (size_t i = 0; i < m_nAllocators; ++i)
  {
    if (m_allocator[i].releaseBlock(p))
    {
      break;
    }
  }
}

MemoryPool::MemoryPool(const size_t *blockSize, const size_t nSizes)
{
#ifdef DEBUG
  std::cout << " ** s_pool_heap @ " << static_cast<void *>(s_pool_heap)
            << std::endl;
#endif

  // First run sanity checks on user input
  if (!scrubBlockSizes(blockSize, nSizes))
  {
    return;
  }

  // Ensure the array is sorted to initialize allocators in ascending order
  sortArray(const_cast<size_t *>(blockSize), nSizes);
#ifdef DEBUG
  std::cout << " ** Block sizes [";
  for (size_t i = 0; i < nSizes; i++)
  {
    std::cout << " " << blockSize[i];
  }
  std::cout << " ]" << std::endl << std::endl;
#endif

  // Allocate the allocators which handle allocating blocks for each block size
  m_poolSize = POOLSIZE_BYTES / nSizes;
  for (size_t i = 0; i < nSizes; ++i)
  {
    // Assumption: because number of block sizes provided is assumed to be a
    // power of two, this division will always result in equally sized
    // mini-heaps
    void *p = &s_pool_heap[i * POOLSIZE_BYTES / nSizes];
#ifdef DEBUG
    std::cout << " ** Block[" << i << "]: size = " << blockSize[i] << " @ " << p
              << std::endl;
#endif
    m_allocator[i] = BlockAllocator(blockSize[i], POOLSIZE_BYTES / nSizes,
        reinterpret_cast<MemoryBlock *>(p));
  }

  m_nAllocators = nSizes;
  m_initialized = true;
}

bool MemoryPool::scrubBlockSizes(const size_t *blockSize, const size_t nSizes)
{
  // 1. Ensure that number of block sizes is a power of 2
  if (!isPower2(nSizes))
  {
#ifdef DEBUG
    std::cerr << " !! Number of block sizes " << nSizes
              << " is not a power of 2!" << std::endl;
#endif
    m_initialized = false;
    return false;
  }

  // 2. Ensure that number of block sizes does not exceed BLKCNT_MAX
  if (nSizes > BLKCNT_MAX)
  {
#ifdef DEBUG
    std::cerr << " !! Number of block sizes " << nSizes
              << " larger than the maximum" << BLKCNT_MAX << "!" << std::endl;
#endif
    m_initialized = false;
    return false;
  }

  size_t bytesPerPool = POOLSIZE_BYTES / nSizes;
  for (size_t i = 0; i < nSizes; ++i)
  {
    // 3. Ensure that all block sizes are a power of 2
    if (!isPower2(blockSize[i]))
    {
#ifdef DEBUG
      std::cerr << " !! Block size [" << i << "]: " << blockSize[i]
                << " is not a power of 2!" << std::endl;
#endif
      m_initialized = false;
      return false;
    }

    // 4. Ensure all block sizes are valid given sizes and count
    if (blockSize[i] > bytesPerPool)
    {
#ifdef DEBUG
      std::cerr << " !! Block size [" << i << "]: " << blockSize[i]
                << " is larger than available bytes per pool (" << bytesPerPool
                << ")!" << std::endl;
#endif
      m_initialized = false;
      return false;
    }

    // 5. Ensure that block sizes are larger that smallest memory block
    if (blockSize[i] < sizeof(MemoryBlock *))
    {
#ifdef DEBUG
      std::cerr << " !! Block size [" << i << "]: " << blockSize[i]
                << " is less than minimum block size (" << sizeof(MemoryBlock *)
                << ")!" << std::endl;
#endif
      m_initialized = false;
      return false;
    }
  }

  return true;
}

bool MemoryPool::isPower2(size_t num)
{
  // Handle special case of 0
  if (0 == num)
  {
    return false;
  }

  // If num eventually is equal to 1, then this is a power of 2
  while (num != 1)
  {
    // If num % 2 is ever non-zero, then this is not a power of 2
    if ((num % 2) != 0)
    {
      return false;
    }
    num /= 2;
  }
  return true;
}

void MemoryPool::sortArray(size_t *array, const size_t nElements)
{
  // An inefficient sort for general cases, but this is called at most once and
  // on an array of at most BLKCNT_MAX elements
  for (size_t i = 0; i < nElements; i++)
  {
    for (size_t j = 0; j < nElements - 1; j++)
    {
      if (array[j] > array[i])
      {
        size_t temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      }
    }
  }
}