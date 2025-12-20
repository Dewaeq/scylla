add_library(common
  src/lcm_node.cpp
)

target_link_libraries(common PUBLIC lcm)

target_include_directories(common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
