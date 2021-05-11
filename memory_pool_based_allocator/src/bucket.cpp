#include "bucket.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

bucket::bucket(std::size_t block_size, std::size_t block_count)
    : BlockSize{block_size}, BlockCount{block_count}
{
    const auto data_size = BlockSize * BlockCount;
    m_data = static_cast<std::byte *>(std::malloc(data_size));
    assert(m_data != nullptr);
    const auto ledger_size = 1 + ((BlockCount - 1) / 8);
    m_ledger = static_cast<std::byte *>(std::malloc(ledger_size));
    assert(m_ledger != nullptr);
    std::memset(m_data, 0, data_size);
    std::memset(m_ledger, 0, ledger_size);
}

bucket::~bucket()
{
    std::free(m_data);
    std::free(m_ledger);
}

void *bucket::allocate(std::size_t bytes) noexcept
{
    // Calculate the required number of blocks
    const auto n = 1 + ((bytes - 1) / BlockSize);

    const auto index = find_contiguous_blocks(n);
    if (index == BlockCount)
    {
        return nullptr;
    }

    set_blocks_in_use(index, n);

    return m_data + (index * BlockSize);
}

void bucket::deallocate(void *ptr, std::size_t bytes) noexcept
{
    const auto p = static_cast<std::byte *>(ptr);
    const std::size_t dist = static_cast<std::size_t>(p - m_data);
    // Calculate block index from pointer distance
    const auto index = dist / BlockSize;
    // Calculate the required number of blocks
    const auto n = 1 + ((bytes - 1) / BlockSize);
    // Update the ledger
    set_blocks_free(index, n);
}

std::size_t bucket::find_contiguous_blocks(std::size_t n) const noexcept
{
    // todo:
    return 0;
}

void set_blocks_in_use(std::size_t index, std::size_t n) noexcept
{
    // todo:
}

void set_blocks_free(std::size_t index, std::size_t n) noexcept
{
    // todo:
}
