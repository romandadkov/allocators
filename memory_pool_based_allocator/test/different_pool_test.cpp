#include <gtest/gtest.h>
#include <list>
#include <scoped_allocator>
#include <vector>

#include "static_pool_allocator.h"

// static constexpr auto ListAllocId = 1;
// static constexpr auto VectorAllocId = 2;
// using list = std::list<int, static_pool_allocator<int, ListAllocId>>;
// using list_allocator = typename list::allocator_type;
// using vector_allocator = static_pool_allocator<list, VectorAllocId>;
// using scoped_adaptor = std::scoped_allocator_adaptor<vector_allocator, list_allocator>;
// using vector = std::vector<list, scoped_adaptor>;
// // Vector and list allocate from different memory pools (1 and 2 respectively)
TEST(Test_different_pool, vector_creation)
{
    // vector v;
    // v.emplace_back(5U, 42);
}
