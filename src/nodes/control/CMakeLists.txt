add_executable(control_node
  src/main.cpp
  src/control_node.cpp
)

target_include_directories(control_node PRIVATE
  include
)

target_link_libraries(control_node PRIVATE
  common
  motor_driver
)
