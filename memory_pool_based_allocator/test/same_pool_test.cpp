#include <gtest/gtest.h>
#include <list>
#include <vector>

#include "static_pool_allocator.h"

static constexpr auto AllocId = 1;
using list = std::list<int, static_pool_allocator<int, AllocId>>;
using vector = std::vector<list, static_pool_allocator<list, AllocId>>;
// Both vector and list allocate from the pool with id 1
TEST(Test_same_pool, vector_creation)
{
    vector v;
    v.emplace_back(5U, 42);
}
