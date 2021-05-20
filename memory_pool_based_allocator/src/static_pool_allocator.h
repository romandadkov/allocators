#pragma once

#include <cassert>
#include <memory>
#include <memory_resource>
#include <iostream>

#include "memory_pool.h"

using allocator_type = std::allocator<int>;

namespace instrument
{
    template <std::size_t id, typename T, std::size_t size>
    void type_reg() {}
} // namespace instrument

template <typename T = std::uint8_t, std::size_t id = 0>
class static_pool_allocator
{
public:
    template <typename U>
    struct rebind
    {
        using other = static_pool_allocator<U, id>;
    };

    using value_type = T;

    static_pool_allocator() noexcept : m_upstream_resource{std::pmr::get_default_resource()} {}
    static_pool_allocator(std::pmr::memory_resource *res) noexcept : m_upstream_resource{res} {}

    template <typename U>
    static_pool_allocator(const static_pool_allocator<U, id> &other) noexcept
        : m_upstream_resource{other.upstream_resource()} {}

    template <typename U>
    static_pool_allocator &operator=(const static_pool_allocator<U, id> &other) noexcept
    {
        m_upstream_resource = other.upstream_resource();
        return *this;
    }

    std::pmr::memory_resource *
    upstream_resource() const noexcept
    {
        return m_upstream_resource;
    }

    static bool initialize_memory_pool() noexcept { return memory_pool::initialize<id>(); }

    using pointer = T *;
    using size_type = std::size_t;

    pointer allocate(size_type n, const void * = 0)
    {
        instrument::type_reg<id, T, sizeof(T)>();
        if constexpr (memory_pool::is_defined<id>())
        {
            return static_cast<T *>(memory_pool::allocate<id>(sizeof(T) * n));
        }
        else if (m_upstream_resource != nullptr)
        {
            return static_cast<T *>(m_upstream_resource->allocate(sizeof(T) * n, alignof(T)));
        }
        else
        {
            throw std::bad_alloc{};
        }
    }

    void deallocate(T *ptr, size_type n)
    {
        if constexpr (memory_pool::is_defined<id>())
        {
            memory_pool::deallocate<id>(ptr, sizeof(T) * n);
        }
        else if (m_upstream_resource != nullptr)
        {
            m_upstream_resource->deallocate(ptr, sizeof(T) * n, alignof(T));
        }
        else
        {
            assert(false);
        }
    }

private:
    std::pmr::memory_resource *m_upstream_resource;
};
