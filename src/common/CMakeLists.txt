add_library(scylla_common
  src/lcm_node.cpp
)

target_link_libraries(scylla_common PUBLIC lcm)

target_include_directories(scylla_common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
