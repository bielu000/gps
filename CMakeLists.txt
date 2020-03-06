cmake_minimum_required(VERSION 3.12)
project(nmea_gps)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


add_executable(nmea_gps gps_test.cpp gps.cpp)

target_link_libraries(nmea_gps gtest gtest_main pthread)