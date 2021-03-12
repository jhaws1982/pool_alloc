#include "BlockAllocator.hpp"

#ifdef DEBUG
#include <iostream>
#endif

BlockAllocator::BlockAllocator(size_t blockSize,
    uint32_t numBytes,
    MemoryBlock *p)
    : m_blockSize(blockSize), m_numBlocks(numBytes / blockSize), m_freeBlock(p)
{
#ifdef DEBUG
  std::cout << " ** Allocator[" << m_blockSize << "] @ " << m_freeBlock
            << " with " << m_numBlocks << " blocks" << std::endl;
#endif

  auto block = m_freeBlock;
  m_startBlock = m_freeBlock;
#ifdef DEBUG
  std::cout << "\tBlock address: " << block << std::endl;
#endif
  for (uint32_t i = 1; i < m_numBlocks; ++i)
  {
    block->m_next = block + m_blockSize / sizeof(block);
    block = block->m_next;
#ifdef DEBUG
    std::cout << "\tBlock address: " << block << std::endl;
#endif
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif

  // Ensure final block has next block set to nullptr
  block->m_next = nullptr;
}

BlockAllocator::BlockAllocator(const BlockAllocator &ma)
{
  m_blockSize = ma.m_blockSize;
  m_numBlocks = ma.m_numBlocks;
  m_freeBlock = ma.m_freeBlock;
  m_startBlock = ma.m_startBlock;
}

BlockAllocator &BlockAllocator::operator=(const BlockAllocator &ma)
{
  m_blockSize = ma.m_blockSize;
  m_numBlocks = ma.m_numBlocks;
  m_freeBlock = ma.m_freeBlock;
  m_startBlock = ma.m_startBlock;

  return *this;
}

MemoryBlock *BlockAllocator::allocateBlock()
{
  // Scoped lock to protect updates to block pointer
  std::lock_guard<std::mutex> lk(m_lock);

  // Save the current block for allocation off to return (can be nullptr)
  auto p = m_freeBlock;

  // Move the block pointer to the next block if not nullptr
  if (m_freeBlock)
  {
    m_freeBlock = m_freeBlock->m_next;
  }

  return p;
}

bool BlockAllocator::releaseBlock(void *p)
{
  bool released = false;

  // Calculate whether the provided address falls within the allocator space
  size_t diff = reinterpret_cast<uint8_t *>(p) -
                reinterpret_cast<uint8_t *>(m_startBlock);
#if defined(DEBUG) && 0
  std::cout << "p: " << p << std::endl;
  std::cout << "a: " << m_startBlock << std::endl;
  std::cout << "p - allocator[i].start = " << diff << " < "
            << (m_blockSize * m_numBlocks) << std::endl;
#endif

  // Check that the provided address is inside the memory block of the allocator
  if ((p >= m_startBlock) && (diff < (m_blockSize * m_numBlocks)))
  {
    released = true;
    MemoryBlock *block = reinterpret_cast<MemoryBlock *>(p);

    // Scoped lock to protect updates to block pointer
    std::lock_guard<std::mutex> lk(m_lock);
    // Link the released block to the beginning of the list
    block->m_next = m_freeBlock;
    // Set the next block pointer at the recently released block
    m_freeBlock = block;

#ifdef DEBUG
    std::cout << " ** Freed block @ " << p << " back to allocator of size "
              << m_blockSize << std::endl;
    showFree();
#endif
  }

  return released;
}

void BlockAllocator::showFree()
{
#ifdef DEBUG
#if 0
  std::cout << " ** Free blocks of size " << m_blockSize << std::endl;
  auto block = m_freeBlock;
  while (nullptr != block)
  {
    std::cout << "\tBlock @ " << block << std::endl;
    block = block->m_next;
  }
  std::cout << std::endl;
#else
  int i = 0;
  auto block = m_freeBlock;
  while (nullptr != block)
  {
    i++;
    block = block->m_next;
  }
  std::cout << " ** " << i << " free blocks of size " << m_blockSize
            << std::endl
            << std::endl;
#endif
#endif
}
