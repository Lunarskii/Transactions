cmake_minimum_required(VERSION 3.22)
project(A6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)

link_directories(./)
include_directories(include)
find_package(GTest REQUIRED)
include(CTest)
enable_testing()
include("${CMAKE_SOURCE_DIR}/tests.cmake")


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -Werror -Wextra -pedantic)
elseif(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    add_compile_options(-g --coverage)
    add_link_options(--coverage)
endif()


add_executable(tests
        include/common/command.h
        include/common/command_invoker.h
        include/common/storage_interface.h
        include/common/storage_struct.h
        include/common/timer.h
        include/common/research.h
        include/common/data_generator.h
        sources/common/storage_struct.cc
        sources/common/data_generator.cc

        include/wrapper/container_wrapper.h
        include/wrapper/container_wrapper.tpp

        include/hash_table/hash_table.h
        include/hash_table/hash_table.tpp

        include/rbtree/rbtree.h
        include/rbtree/kvtree.h

        include/bpt/b_plus_tree.h
        include/bpt/b_plus_tree.tpp
        include/bpt/b_plus_tree_node.tpp

        include/tests/test_core.h
        include/tests/test_hash_table.h
        include/tests/test_b_plus_tree.h
        include/tests/test_container_wrapper.h
        include/tests/test_rb_tree.h
        sources/tests/test_hash_table.cc
        sources/tests/test_b_plus_tree.cc
        sources/tests/test_container_wrapper.cc
        sources/tests/test_rb_tree.cc
        sources/tests/test_rb_tree_base.cc
)
target_compile_definitions(tests PRIVATE TEST_MATERIALS_PATH="${CMAKE_SOURCE_DIR}/sources/tests/materials")
target_link_libraries(tests GTest::gtest_main)

add_executable(Transactions
        include/common/command.h
        include/common/command_invoker.h
        include/common/storage_interface.h
        include/common/storage_struct.h
        include/common/timer.h
        include/common/research.h
        include/common/data_generator.h
        sources/common/storage_struct.cc
        sources/common/data_generator.cc

        include/wrapper/container_wrapper.h
        include/wrapper/container_wrapper.tpp

        include/hash_table/hash_table.h
        include/hash_table/hash_table.tpp

        include/rbtree/rbtree.h
        include/rbtree/kvtree.h

        include/bpt/b_plus_tree.h
        include/bpt/b_plus_tree.tpp
        include/bpt/b_plus_tree_node.tpp

        include/common/cli.h
        sources/common/cli.cc
        sources/common/main.cc
)