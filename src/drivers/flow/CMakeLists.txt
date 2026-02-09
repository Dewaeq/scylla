add_library(flow_driver
  src/flow_driver.cpp
)

target_link_libraries(flow_driver PRIVATE
  wiringPi
)

target_include_directories(flow_driver PUBLIC
  include
)
