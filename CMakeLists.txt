cmake_minimum_required(VERSION 3.30)
project(scylla CXX)

find_package(lcm REQUIRED)

add_subdirectory(lcm_types)
add_subdirectory(src)
