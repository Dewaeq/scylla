cmake_minimum_required(VERSION 3.10)
project(scylla LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# configure lcm
find_package(PkgConfig REQUIRED)
pkg_check_modules(LCM REQUIRED lcm)

# generate c++ types from lcm files
file(GLOB LCM_FILES ${CMAKE_SOURCE_DIR}/lcm_types/*.lcm)
execute_process(
  COMMAND lcm-gen -px ${LCM_FILES} --cpp-hpath ${CMAKE_SOURCE_DIR}/include --ppath ${CMAKE_SOURCE_DIR}/python/
)

include_directories(${LCM_INCLUDE_DIRS})

# configure the rplidar sdk (this took an embarrassingly long amout of time :( )
include_directories(
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/include/
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src
)

file(GLOB RPLIDAR_SDK_SRC
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src/arch/linux/*.cpp
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src/hal/*.cpp
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src/*.cpp
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src/dataunpacker/*.cpp
  ${CMAKE_SOURCE_DIR}/external/rplidar_sdk/sdk/src/dataunpacker/unpacker/*.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(rplidar_sdk ${RPLIDAR_SDK_SRC})

# define modules
add_subdirectory(${CMAKE_SOURCE_DIR}/nodes/lidar)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  add_subdirectory(${CMAKE_SOURCE_DIR}/nodes/control)
else()
  message(STATUS "skipping control node (not building for rpi)")
endif()
