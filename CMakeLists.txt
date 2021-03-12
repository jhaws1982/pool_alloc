# CMake file for building the memory pool allocator
cmake_minimum_required (VERSION 3.12)

set (std_libs -pthread -lrt -lm -lstdc++)
set (warning_flags "-Wall -Wno-unused-variable -pedantic")
set (c_language_flags "-std=c17")
set (cxx_language_flags "-std=c++17")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warning_flags} ${c_language_flags}" )
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warning_flags} ${cxx_language_flags}" )

##############################################################################
project(mempool)
set(${PROJECT_NAME}_srcs
    src/BlockAllocator.hpp
    src/BlockAllocator.cpp
    src/MemoryPool.hpp
    src/MemoryPool.cpp
    src/pool_alloc.h
    src/pool_alloc.cpp
)

# add_compile_definitions(DEBUG)
include_directories(${PROJECT_NAME} src)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_srcs})
target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}LIB_SOURCE_DIR})


##############################################################################
project(testpool)
add_executable(${PROJECT_NAME} tests/general.cpp)
target_link_libraries(${PROJECT_NAME} mempool)


##############################################################################
project(testpool_c)
add_executable(${PROJECT_NAME} tests/general.c)
target_link_libraries(${PROJECT_NAME} mempool)


##############################################################################
project(allocFailure)
add_executable(${PROJECT_NAME} tests/allocationFailure.cpp)
target_link_libraries(${PROJECT_NAME} mempool)