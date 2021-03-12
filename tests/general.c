#include <stdio.h>
#include "pool_alloc.h"

void *allocationTest(const size_t n, bool expected)
{
  void *block = pool_malloc(n);

  if (!block && expected)
  {
    printf(" -- FAIL: allocation failed!\n");
  }
  else if (!block && !expected)
  {
    printf(" -- PASS: allocation failed!\n");
  }
  else if (block && expected)
  {
    printf(" -- PASS: allocation successful!\n");
  }
  else if (block && !expected)
  {
    printf(" -- FAIL: allocation successful!\n");
  }

  return block;
}

int main()
{
  size_t blksz[4] = {2048, 8192, 4096, 16384};  // 8, 2, 4, 1 blocks of
  pool_init(blksz, sizeof(blksz) / sizeof(size_t));

  void *b2048_1 = allocationTest(32, true);    // 2048
  void *b2048_2 = allocationTest(32, true);    // 2048
  void *b2048_3 = allocationTest(32, true);  // 2048
  void *b2048_4 = allocationTest(32, true);    // 2048
  void *b2048_5 = allocationTest(32, true);    // 2048
  void *b2048_6 = allocationTest(32, true);    // 2048
  void *b2048_7 = allocationTest(32, true);    // 2048
  void *b2048_8 = allocationTest(32, true);    // 2048

  void *b4096_1 = allocationTest(32, true);  // 4096

  if (b2048_3)
  {
    pool_free(b2048_3);
  }

  void *b2048_9 = allocationTest(32, true);  // 2048
  void *b4096_2 = allocationTest(32, true);  // 4096
  void *b4096_3 = allocationTest(32, true);  // 4096
  void *b4096_4 = allocationTest(32, true);  // 4096

  if (b4096_2)
  {
    pool_free(b4096_2);
  }
  if (b4096_3)
  {
    pool_free(b4096_3);
  }

  void *b4096_5 = allocationTest(32, true);   // 4096
  void *b4096_6 = allocationTest(32, true);   // 4096
  void *b8192_1 = allocationTest(32, true);   // 8192
  void *b8192_2 = allocationTest(32, true);   // 8192
  void *b16384_1 = allocationTest(32, true);  // 16384

  if (b8192_1)
  {
    pool_free(b8192_1);
  }

  void *b8192_3 = allocationTest(32, true);  // 8192

  if (b4096_5)
  {
    pool_free(b4096_5);
  }
  if (b4096_6)
  {
    pool_free(b4096_6);
  }

  void *b4096_7 = allocationTest(32, true);  // 4096
  void *b4096_8 = allocationTest(32, true);  // 4096
  void *b16384_2 = allocationTest(32, false);  // allocation failure

  int x = 73;
  void *xptr = &x;
  pool_free(xptr);  // no-op because not managed by pool (no "freed" print)
}