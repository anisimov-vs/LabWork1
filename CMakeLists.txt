cmake_minimum_required(VERSION 3.16)
project(bitmap_labwork VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main executable
add_executable(main
    src/main.cpp
    src/bitmap.cpp
    src/util.cpp
)

target_include_directories(main PRIVATE include)

# Dependencies for tests
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

# Tests
add_executable(test_bitmap
    tests/test_bitmap.cpp
    src/bitmap.cpp
    src/util.cpp
)
target_include_directories(test_bitmap PRIVATE include)
target_link_libraries(test_bitmap PRIVATE GTest::gtest_main Threads::Threads)
add_test(NAME bitmap_tests COMMAND test_bitmap)

add_custom_command(TARGET test_bitmap PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/output
)

add_custom_command(TARGET test_bitmap POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/images/24.bmp
        ${CMAKE_BINARY_DIR}/24.bmp
) 
