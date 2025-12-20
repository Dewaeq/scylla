file(GLOB LCM_FILES "*.lcm")

set(LCM_GEN_INCLUDE_DIR ${CMAKE_BINARY_DIR}/lcm_types)

add_custom_command(
  OUTPUT ${LCM_GEN_INCLUDE_DIR}/car/flow_t.hpp
  COMMAND lcm-gen -x ${LCM_FILES} --cpp-std=c++11 --outpath ${LCM_GEN_INCLUDE_DIR}
  DEPENDS ${LCM_FILES}
  COMMENT "Generating LCM C++ headers..."
)

# Create a "Interface Library"
# Any executable that links 'lcm_messages' will automatically know where the headers are
add_library(lcm_messages INTERFACE)
target_include_directories(lcm_messages INTERFACE ${LCM_GEN_INCLUDE_DIR})
# We add a dummy custom target to force the command to run
add_custom_target(lcm_gen_target DEPENDS ${LCM_GEN_INCLUDE_DIR}/car/flow_t.hpp)
add_dependencies(lcm_messages lcm_gen_target)
