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
    std::size_t index{0};
    std::size_t count{0};
    for (; (index < BlockCount) && (count < n); index++)
    {
        const auto i = (index / 8);
        const auto j = (index % 8);
        const auto in_use = (static_cast<unsigned char>(m_ledger[i]) >> j) & 1U;

        if (in_use)
        {
            count = 0;
        }
        else
        {
            count++;
        }
    }

    assert(n == count);

    return index;
}

void bucket::set_blocks_in_use(std::size_t index, std::size_t n) noexcept
{
    for (int k = 0; k < n; ++k)
    {
        const auto i = ((index + k) / 8);
        const auto j = ((index + k) % 8);
        m_ledger[i] |= std::byte(1UL << j);
    }
}

void bucket::set_blocks_free(std::size_t index, std::size_t n) noexcept
{
    for (int k = 0; k < n; ++k)
    {
        const auto i = ((index + k) / 8);
        const auto j = ((index + k) % 8);
        m_ledger[i] &= std::byte(~(1UL << j));
    }
}

bool bucket::belongs(void *ptr) const noexcept
{
    if ((unsigned long long)ptr >= (unsigned long long)m_data &&
        (unsigned long long)ptr < (unsigned long long)m_data + (unsigned long long)(BlockSize * BlockCount))
    {
        return true;
    }
    return false;
}
