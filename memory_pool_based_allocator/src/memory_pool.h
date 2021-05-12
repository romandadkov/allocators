#pragma once

#include "bucket.h"

#include <array>
#include <tuple>

namespace memory_pool
{

    // Default implementation defines a pool with no buckets
    template <std::size_t id>
    struct bucket_descriptors
    {
        using type = std::tuple<>;
    };

    // Specializing instances
    struct bucket_cfg16
    {
        static constexpr std::size_t BlockSize = 16;
        static constexpr std::size_t BlockCount = 10000;
    };

    struct bucket_cfg32
    {
        static constexpr std::size_t BlockSize = 32;
        static constexpr std::size_t BlockCount = 10000;
    };

    struct bucket_cfg1024
    {
        static constexpr std::size_t BlockSize = 1024;
        static constexpr std::size_t BlockCount = 50000;
    };

    // Creating the memory pool(s)
    template <>
    struct bucket_descriptors<1>
    {
        using type = std::tuple<bucket_cfg16, bucket_cfg32, bucket_cfg1024>;
    };

    template <std::size_t id>
    using bucket_descriptors_t = typename bucket_descriptors<id>::type;

    template <std::size_t id>
    static constexpr std::size_t bucket_count = std::tuple_size<bucket_descriptors_t<id>>::value;

    template <std::size_t id>
    using pool_type = std::array<bucket, bucket_count<id>>;

    template <std::size_t id, std::size_t Idx>
    struct get_size : std::integral_constant<std::size_t, std::tuple_element_t<Idx, bucket_descriptors_t<id>>::BlockSize>
    {
    };

    template <std::size_t id, std::size_t Idx>
    struct get_count : std::integral_constant<std::size_t, std::tuple_element_t<Idx, bucket_descriptors_t<id>>::BlockCount>
    {
    };

    template <std::size_t id, std::size_t... Idx>
    auto &get_instance(std::index_sequence<Idx...>) noexcept
    {
        static pool_type<id> instance{{{get_size<id, Idx>::value, get_count<id, Idx>::value}...}};
        return instance;
    }

    template <std::size_t id>
    auto &get_instance() noexcept
    {
        return get_instance<id>(std::make_index_sequence<bucket_count<id>>());
    }

    struct info
    {
        std::size_t index{0};       // which bucket?
        std::size_t block_count{0}; // how many blocks would the allocation take from the bucket?
        std::size_t waste{0};       // how much memory would be wasted?

        bool operator<(const info &other) const noexcept
        {
            return (waste == other.waste) ? block_count < other.block_count : waste < other.waste;
        }
    };

    template <std::size_t id>
    [[nodiscard]] void *allocate(std::size_t bytes)
    {
        auto &pool = get_instance<id>();
        std::array<info, bucket_count<id>> deltas;
        std::size_t index = 0;
        for (const auto &bucket : pool)
        {
            deltas[index].index = index;
            if (bucket.BlockSize >= bytes)
            {
                deltas[index].waste = bucket.BlockSize - bytes;
                deltas[index].block_count = 1;
            }
            else
            {
                const auto n = 1 + ((bytes - 1) / bucket.BlockSize);
                const auto storage_required = n * bucket.BlockSize;
                deltas[index].waste = storage_required - bytes;
                deltas[index].block_count = n;
            }
            ++index;
        }

        sort(deltas.begin(), deltas.end()); // std:::sort is allowed to allocate

        for (const auto &d : deltas)
        {
            if (auto ptr = pool[d.index].allocate(bytes); ptr != nullptr)
            {
                return ptr;
            }
        }

        throw std::bad_alloc{};
    }

    template <std::size_t id>
    void deallocate(void *ptr, std::size_t bytes)
    {
        auto &pool = get_instance<id>();

        for (const auto &bucket : pool)
        {
            if (bucket.belongs(ptr))
            {
                bucket.deallocate(ptr, bytes);
                return;
            }
        }
    }

    template <std::size_t id>
    constexpr bool is_defined() noexcept
    {
        return bucket_count<id> != 0;
    }

    template <std::size_t id>
    bool initialize() noexcept
    {
        (void)get_instance<id>();
        return is_defined<id>();
    }

} // namespace memory_pool
