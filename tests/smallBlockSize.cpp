#include <iostream>
#include "MemoryPool.hpp"

static size_t blksz[4] = {2, 4, 8, 16};  // 8, 2, 4, 1 blocks of
                                                     // each
int main()
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));

  if (!pool.isInitialized())
  {
    std::cout << " -- PASS: initialization failed as expected!" << std::endl;
  }
  else
  {
    std::cout << " -- FAIL: initialization succeeded when should have failed!"
              << std::endl;
  }

  return pool.isInitialized();
}