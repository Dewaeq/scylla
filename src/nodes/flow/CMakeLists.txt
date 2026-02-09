add_executable(flow_node
  src/main.cpp
  src/flow_node.cpp
)

target_include_directories(flow_node PRIVATE
  include
)

target_link_libraries(flow_node PRIVATE
  common
  flow_driver
)
