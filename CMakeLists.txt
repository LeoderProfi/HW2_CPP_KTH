cmake_minimum_required(VERSION 3.15)

project(Assignment2 VERSION 1.0
                    DESCRIPTION "Assignment #2"
                    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(UTEST_HEADER_DIR ${CMAKE_BINARY_DIR}/external/utest)
file(DOWNLOAD
        https://raw.githubusercontent.com/sheredom/utest.h/master/utest.h
        ${UTEST_HEADER_DIR}/utest.h
)

set(UTILS_SOURCES
    src/utils/mpl_writer.hpp
    src/utils/point_reader.hpp
    src/utils/random_points.hpp
    src/utils/timer.hpp)

add_executable(main src/main.cpp src/geometry.hpp)
target_sources(main PRIVATE ${UTILS_SOURCES})
# add_executable(tests test/test_integration.cpp src/integration.cpp)

# target_include_directories(tests PRIVATE ${UTEST_HEADER_DIR})
#
# enable_testing()
# add_test(NAME IntegrationTest COMMAND tests)
