#pragma once

#include <memory>
#include <memory_resource>

#include "memory_pool.h"

using allocator_type = std::allocator<int>;

template <typename T = std::uint8_t, std::size_t id = 0>
class static_pool_allocator
{
public:
    template <typename U>
    struct rebind
    {
        using other = static_pool_allocator<U, id>;
    };

    static_pool_allocator() noexcept : m_upstream_resource{std::pmr::get_default_resource()} {}
    static_pool_allocator(std::pmr::memory_resource *res) noexcept : m_upstream_resource{res} {}

    template <typename U>
    static_pool_allocator(const static_pool_allocator<U, id> &other) noexcept
        : m_upstream_resource{other.m_upstream_resource()} {}

    template <typename U>
    static_pool_allocator &operator=(const static_pool_allocator<U, id> &other) noexcept
    {
        m_upstream_resource = other.m_upstream_resource();
        return *this;
    }

    static bool initialize_memory_pool() noexcept { return memory_pool::initialize<id>(); }

private:
    std::pmr::memory_resource *m_upstream_resource;
};
