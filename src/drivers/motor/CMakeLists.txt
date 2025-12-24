add_library(motor_driver
  src/motor_driver.cpp
)

target_link_libraries(motor_driver PUBLIC
  wiringPi
)

target_include_directories(motor_driver PUBLIC
  include
)
