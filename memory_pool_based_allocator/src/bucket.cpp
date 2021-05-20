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
    // todo:
    return true;
}


/*
https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit

#include <iostream>
#include <cstddef>
#include <bitset>
#include <cassert>
#include <cstdlib>
#include <cstring>


using namespace std;

void print_b(std::byte b)
{
    auto a = static_cast<unsigned char>(b);
    std::bitset<8> x(a);
    std::cout << x << '\n';
}

int main()
{
    auto b = std::byte{5};
    //print_b(b);
    b |= std::byte(1U << 1);
    //print_b(b);

    cout << "array" << endl;

    std::byte *pb{nullptr};
    auto BlockCount = 13;
    auto index = 1;
    auto n = 9;
    const auto pb_size = 1 + ((BlockCount - 1) / 8);
    pb = static_cast<std::byte *>(std::malloc(pb_size));
    std::memset(pb, 0, pb_size);
    cout << pb_size << endl;

    for (int k = 0; k < pb_size; ++k) {
        print_b(pb[k]);
    }

    for (int k = 0; k < n; ++k) {
        auto i = ((index + k) / 8);
        auto j = ((index + k) % 8);
        pb[i] |= std::byte(1UL << j);
    }

    cout << "in use" << endl;
    for (int k = 0; k < pb_size; ++k) {
        print_b(pb[k]);
    }


    index = 7;
    n = 2;
    for (int k = 0; k < n; ++k)
    {
        auto i = ((index + k) / 8);
        auto j = ((index + k) % 8);
        pb[i] &= std::byte(~(1UL << j));
    }

    cout << "free" << endl;
    for (int k = 0; k < pb_size; ++k) {
        print_b(pb[k]);
    }


    cout << "find" << endl;
    n = 5;
    index = 0;
    int count = 0;
    for (; (index < BlockCount) && (count < n); index++) {
        auto i = (index / 8);
        auto j = (index % 8);
        const auto in_use = (static_cast<unsigned char>(pb[i]) >> j) & 1U;

        if (in_use) {
            count = 0;
        }
        else {
            count++;
        }
    }

    if (bool(n == count)) {
    cout << "was found index " << index << endl;
    }
    else {
        cout << "was not found"<< endl;
    }

    return 0;
}
*/