#include <iostream>
#include "MemoryPool.hpp"

static size_t blksz[4] = {2048, 8192, 4096, 16384};  // 8, 2, 4, 1 blocks of
                                                     // each
int main()
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));
  auto b1 = pool.allocate(16384);  // success
  auto b2 = pool.allocate(16384);   // failure

  if (!b2)
  {
    std::cout << " -- PASS: allocation failed as expected!" << std::endl;
  }
  else
  {
    std::cout << " -- FAIL: allocation succeeded when should have failed!"
              << std::endl;
  }

  return !(b2 == nullptr);
}