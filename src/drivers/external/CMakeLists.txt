set(RPLIDAR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rplidar_sdk)

file(GLOB RPLIDAR_SOURCES
  ${RPLIDAR_DIR}/sdk/src/arch/linux/*.cpp
  ${RPLIDAR_DIR}/sdk/src/hal/*.cpp
  ${RPLIDAR_DIR}/sdk/src/*.cpp
  ${RPLIDAR_DIR}/sdk/src/dataunpacker/*.cpp
  ${RPLIDAR_DIR}/sdk/src/dataunpacker/unpacker/*.cpp
)

add_library(rplidar_sdk STATIC ${RPLIDAR_SOURCES})

target_include_directories(rplidar_sdk PUBLIC
  ${RPLIDAR_DIR}/sdk/include
  ${RPLIDAR_DIR}/sdk/src
)
