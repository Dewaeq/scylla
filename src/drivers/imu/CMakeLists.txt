add_library(imu_driver
  src/imu_driver.cpp
)

target_link_libraries(imu_driver PRIVATE
  wiringPi
)

target_include_directories(imu_driver PUBLIC
  include
)
