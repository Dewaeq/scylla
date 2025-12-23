cmake_minimum_required(VERSION 3.30)
project(scylla CXX)

find_package(lcm REQUIRED)

add_subdirectory(src)

file(GLOB LCM_FILES lcm_types/*.lcm)
set(LCM_GEN_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
# dummy file that acts as a flag. If it's missing or older than the .lcm files,
# CMake knows it needs to run the generator again
set(LCM_GEN_STAMP ${CMAKE_CURRENT_BINARY_DIR}/lcm_gen.stamp)

add_custom_command(
  OUTPUT ${LCM_GEN_STAMP}
  COMMAND lcm-gen -x ${LCM_FILES} --cpp-hpath ${LCM_GEN_INCLUDE_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${LCM_GEN_STAMP}
  DEPENDS ${LCM_FILES}
  COMMENT "generating LCM C++ headers..."
)

# add a dummy custom target to force the command to run
add_custom_target(lcm_gen_target ALL DEPENDS ${LCM_GEN_STAMP})

add_library(lcm_messages INTERFACE)
target_include_directories(lcm_messages INTERFACE ${LCM_GEN_INCLUDE_DIR})
add_dependencies(lcm_messages lcm_gen_target)
