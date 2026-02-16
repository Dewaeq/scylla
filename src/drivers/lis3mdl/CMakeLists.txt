add_library(lis3mdl_driver
  src/lis3mdl_driver.cpp
)

target_link_libraries(lis3mdl_driver PRIVATE
  wiringPi
)

target_include_directories(lis3mdl_driver PUBLIC
  include
)
