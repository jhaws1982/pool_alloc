#include <iostream>
#include <vector>
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

std::vector<void *> fillPool(const size_t n)
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));
  void *block = nullptr;
  std::vector<void *> b;
  do
  {
    block = pool.allocate(n);
    if (block)
    {
      b.emplace_back(block);
    }
  } while (block);
  return b;
}

void releasePool(std::vector<void *> b)
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));
  for (auto block : b)
  {
    pool.release(block);
  }
}

int main()
{
  auto &pool = MemoryPool::getPool(blksz, sizeof(blksz) / sizeof(size_t));

  // Fill pools and test allocation failures
  auto b16384 = fillPool(16384);
  auto b8192 = fillPool(8192);
  auto b4096 = fillPool(4096);
  auto b2048 = fillPool(2048);

  // All allocations should fail
  std::cout << " ** Testing allocation failure:" << std::endl;
  allocationTest(32, false);
  allocationTest(73, false);
  allocationTest(1234, false);
  allocationTest(2048, false);
  allocationTest(3500, false);
  allocationTest(4096, false);
  allocationTest(7000, false);
  allocationTest(8192, false);
  allocationTest(12345, false);
  allocationTest(16384, false);
  allocationTest(16385, false);
  allocationTest(32768, false);

  // Release a single block and test new allocation
  releasePool(b16384);
  auto b16384_a = allocationTest(16384);
  pool.release(b16384_a);

  b16384_a = allocationTest(8192);
  pool.release(b16384_a);
  b16384_a = allocationTest(4096);
  pool.release(b16384_a);
  b16384_a = allocationTest(2048);
  pool.release(b16384_a);
  b16384_a = allocationTest(32);
  pool.release(b16384_a);

  // Release memory to start new test
  releasePool(b2048);
  releasePool(b4096);
  releasePool(b8192);
  releasePool(b16384);
  releasePool(b16384);
  releasePool(b16384);
  releasePool(b16384);

  // Test allocations and lots of releasing, ensuring addresses received back
  // are valid and the expected values
  std::cout << std::endl
            << " ** Testing filled pools, releases, and sane new allocs:"
            << std::endl;
  auto b2048_1 = allocationTest(32);  // 2048
  auto b2048_2 = allocationTest(32);  // 2048
  auto b2048_3 = allocationTest(32);  // 2048
  auto b2048_4 = allocationTest(32);  // 2048
  auto b2048_5 = allocationTest(32);  // 2048
  auto b2048_6 = allocationTest(32);  // 2048
  auto b2048_7 = allocationTest(32);  // 2048
  auto b2048_8 = allocationTest(32);  // 2048

  auto b4096_1 = allocationTest(32);  // 4096

  // Release something in the middle of the smaller pool
  if (b2048_3)
  {
    pool.release(b2048_3);
  }

  // Allocate something in that pool (should get the same address back)
  auto b2048_9 = allocationTest(32);  // 2048
  if (b2048_3 != b2048_9)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }
  auto b4096_2 = allocationTest(32);  // 4096
  auto b4096_3 = allocationTest(32);  // 4096
  auto b4096_4 = allocationTest(32);  // 4096

  // Release two addresses in the middle of the 4096 pool
  if (b4096_2)
  {
    pool.release(b4096_2);
  }
  if (b4096_3)
  {
    pool.release(b4096_3);
  }

  // Ensure that next two allocations match the reverse of the two releases
  auto b4096_5 = allocationTest(32);  // 4096
  if (b4096_5 != b4096_3)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }
  auto b4096_6 = allocationTest(32);  // 4096
  if (b4096_6 != b4096_2)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }

  // Fill 8192 and 16384
  auto b8192_1 = allocationTest(32);   // 8192
  auto b8192_2 = allocationTest(32);   // 8192
  auto b16384_1 = allocationTest(32);  // 16384

  // Release an 8192 and check new allocation
  if (b8192_1)
  {
    pool.release(b8192_1);
  }
  auto b8192_3 = allocationTest(32);  // 8192
  if (b8192_1 != b8192_3)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }

  if (b4096_5)
  {
    pool.release(b4096_5);
  }
  if (b4096_6)
  {
    pool.release(b4096_6);
  }

  auto b4096_7 = allocationTest(32);  // 4096
  if (b4096_6 != b4096_7)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }
  auto b4096_8 = allocationTest(32);  // 4096
  if (b4096_5 != b4096_8)
  {
    std::cout << " !! Addresses do not match, yet they should!\n";
  }
  auto b16384_2 = allocationTest(32, false);  // allocation failure

  int x = 73;
  void *xptr = &x;
  pool.release(xptr);  // no-op because not managed by pool

  // Free entire 4096 pool, testing double release cases
  pool.release(b4096_8);
  pool.release(b4096_7);
  pool.release(b4096_6);
  pool.release(b4096_5);
  pool.release(b4096_4);
  pool.release(b4096_3);  // already released - check for infinite loop in chain
  pool.release(b4096_2);  // already released - check for infinite loop in chain
  pool.release(b4096_1);

  // One more allocation failure check, for good measure
  b4096_2 = allocationTest(4096);
  b4096_3 = allocationTest(4096);
  b4096_7 = allocationTest(4096);
  b4096_8 = allocationTest(4096);
  auto b4096_9 = allocationTest(4096, false);  // allocation failure

  // NOTE: Not bothering to cleanup memory at the end of this test due to good
  // Linux memory management. However, if this test was to be automated on a
  // system without a memory manager and not releasing memory could cause issues
  // then that code can be added to ensure all memory is freed. Also, due to the
  // nature of this allocator (all from a single array on the heap), I struggle
  // to see a case where not bothering to cleanup poses a problem.
}