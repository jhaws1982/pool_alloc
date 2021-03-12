#include <pool_alloc.h>

#include "MemoryPool.hpp"

static MemoryPool *s_pool = nullptr;

extern "C"
{
  bool pool_init(const size_t *block_sizes, size_t block_size_count)
  {
    s_pool = &MemoryPool::getPool(block_sizes, block_size_count);
    return s_pool->isInitialized();
  }

  void *pool_malloc(size_t n) { return s_pool->allocate(n); }

  void pool_free(void *ptr) { s_pool->release(ptr); }
}