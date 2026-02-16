add_executable(imu_node
  src/main.cpp
  src/imu_node.cpp
)

target_include_directories(imu_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imu_node PRIVATE
  common
  imu_driver
)
