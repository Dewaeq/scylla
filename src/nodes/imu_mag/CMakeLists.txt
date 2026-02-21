add_executable(imu_mag_node
  src/main.cpp
  src/imu_mag_node.cpp
)

target_include_directories(imu_mag_node PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imu_mag_node PRIVATE
  common
  lsm6dsox_driver
  lis3mdl_driver
)
