add_executable(lidar_node
  src/main.cpp
  src/lidar_node.cpp
)

target_include_directories(lidar_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lidar_node PRIVATE
  common
  rplidar_sdk
)
