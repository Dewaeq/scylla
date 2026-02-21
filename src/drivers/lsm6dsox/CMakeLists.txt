add_library(lsm6dsox_driver
  src/lsm6dsox_driver.cpp
)

target_link_libraries(lsm6dsox_driver PRIVATE
  wiringPi
)

target_include_directories(lsm6dsox_driver PUBLIC
  include
)
