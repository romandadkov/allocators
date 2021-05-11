add_library(static_pool_allocator)

target_sources(static_pool_allocator
  PRIVATE
    bucket.cpp
    static_pool_allocator.cpp
  PUBLIC
    bucket.h
    static_pool_allocator.h
  )

target_include_directories(static_pool_allocator
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

# we use this to get code coverage
# flags are only valid with the GNU compiler and on Linux
if(CMAKE_CXX_COMPILER_ID MATCHES GNU AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(static_pool_allocator
    PUBLIC
      "--coverage"
    )
  target_link_options(static_pool_allocator
    INTERFACE
      "--coverage"
    )
endif()
