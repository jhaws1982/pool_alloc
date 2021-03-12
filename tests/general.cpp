#include <iostream>
#include "MemoryPool.hpp"

static size_t blksz[4] = {2048, 8192, 4096, 16384};  // 8, 2, 4, 1 blocks of
                                                     // each
void *allocationTest(const size_t n, bool expected = true)
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));
  auto block = pool.allocate(n);

  if (!block && expected)
  {
    std::cout << " -- FAIL: allocation failed!" << std::endl;
  }
  else if (!block && !expected)
  {
    std::cout << " -- PASS: allocation failed!" << std::endl;
  }
  else if (block && expected)
  {
    std::cout << " -- PASS: allocation successful!" << std::endl;
  }
  else if (block && !expected)
  {
    std::cout << " -- FAIL: allocation successful!" << std::endl;
  }

  return block;
}

int main()
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));

  auto b2048_1 = allocationTest(32);  // 2048
  auto b2048_2 = allocationTest(32);  // 2048
  auto b2048_3 = allocationTest(32);  // 2048
  auto b2048_4 = allocationTest(32);  // 2048
  auto b2048_5 = allocationTest(32);  // 2048
  auto b2048_6 = allocationTest(32);  // 2048
  auto b2048_7 = allocationTest(32);  // 2048
  auto b2048_8 = allocationTest(32);  // 2048

  auto b4096_1 = allocationTest(32);  // 4096

  if (b2048_3)
  {
    pool.release(b2048_3);
  }

  auto b2048_9 = allocationTest(32);  // 2048
  auto b4096_2 = allocationTest(32);  // 4096
  auto b4096_3 = allocationTest(32);  // 4096
  auto b4096_4 = allocationTest(32);  // 4096

  if (b4096_2)
  {
    pool.release(b4096_2);
  }
  if (b4096_3)
  {
    pool.release(b4096_3);
  }

  auto b4096_5 = allocationTest(32);   // 4096
  auto b4096_6 = allocationTest(32);   // 4096
  auto b8192_1 = allocationTest(32);   // 8192
  auto b8192_2 = allocationTest(32);   // 8192
  auto b16384_1 = allocationTest(32);  // 16384

  if (b8192_1)
  {
    pool.release(b8192_1);
  }

  auto b8192_3 = allocationTest(32);  // 8192

  if (b4096_5)
  {
    pool.release(b4096_5);
  }
  if (b4096_6)
  {
    pool.release(b4096_6);
  }

  auto b4096_7 = allocationTest(32);   // 4096
  auto b4096_8 = allocationTest(32);   // 4096
  auto b16384_2 = allocationTest(32, false);  // allocation failure

  int x = 73;
  void *xptr = &x;
  pool.release(xptr);  // no-op because not managed by pool (no "freed" print)
}