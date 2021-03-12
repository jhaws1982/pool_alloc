#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * @brief Initialize the pool allocator with a set of block sizes appropriate
   * for the application.
   *
   * @param block_sizes Array of block sizes to configure in pool
   * @param block_size_count Number of block sizes in the size array
   * @return true Pool successfully initialzed
   * @return false Error initializing pool
   */
  bool pool_init(const size_t *block_sizes, size_t block_size_count);

  /**
   * @brief Allocate n bytes from pool
   *
   * @param n Number of bytes to allocate
   * @return void* Pointer to allocated memory, or NULL if unavailable
   */
  void *pool_malloc(size_t n);

  /**
   * @brief Release allocation pointed to by ptr
   *
   * @param ptr Pointer to memory to release back to the pool
   */
  void pool_free(void *ptr);

#ifdef __cplusplus
}
#endif