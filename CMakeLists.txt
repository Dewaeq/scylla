cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(scylla CXX)

find_package(lcm REQUIRED)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)

# configure LCM
file(GLOB LCM_FILES lcm_types/*.lcm)
set(LCM_GEN_INCLUDE_DIR ${CMAKE_BINARY_DIR}/lcm/include)
set(LCM_GEN_PYTHON_DIR ${CMAKE_BINARY_DIR}/lcm/python)
set(LCM_GEN_JAVA_DIR ${CMAKE_BINARY_DIR}/lcm/java)
# dummy file that acts as a flag. If it's missing or older than the .lcm files,
# CMake knows it needs to run the generator again
set(LCM_GEN_STAMP ${CMAKE_CURRENT_BINARY_DIR}/lcm_gen.stamp)

add_custom_command(
  OUTPUT ${LCM_GEN_STAMP}
  COMMAND lcm-gen -x ${LCM_FILES} --cpp-hpath ${LCM_GEN_INCLUDE_DIR}
  COMMAND lcm-gen -p ${LCM_FILES} --ppath ${LCM_GEN_PYTHON_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${LCM_GEN_STAMP}
  DEPENDS ${LCM_FILES}
  COMMENT "generating LCM C++ headers..."
)

# add a dummy custom target to force the command to run
add_custom_target(lcm_gen_target ALL DEPENDS ${LCM_GEN_STAMP})

add_library(lcm_messages INTERFACE)
target_include_directories(lcm_messages INTERFACE ${LCM_GEN_INCLUDE_DIR})
add_dependencies(lcm_messages lcm_gen_target)

option(BUILD_JAVA_TYPES "build jar for lcm-spy" ON)
if(BUILD_JAVA_TYPES)
  find_package(Java COMPONENTS Development)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LCM_JAVA lcm-java)

  if(Java_FOUND AND LCM_JAVA_FOUND)
    include(UseJava)
    pkg_get_variable(LCM_JAR_PATH lcm-java classpath)

    # define the list of java files we expect to be generated
    set(JAVA_FILES "")
    foreach(lcm_file ${LCM_FILES})
      get_filename_component(name ${lcm_file} NAME_WE)
      list(APPEND JAVA_FILES "${LCM_GEN_JAVA_DIR}/scylla_msgs/${name}.java")
    endforeach()

    add_custom_command(
      OUTPUT ${JAVA_FILES}
      COMMAND lcm-gen -j ${LCM_FILES} --jpath ${LCM_GEN_JAVA_DIR}
      DEPENDS ${LCM_FILES}
      COMMENT "generating LCM java source files"
    )

    file(GLOB_RECURSE JAVA_SOURCES "${LCM_GEN_JAVA_DIR}/*.java")

    add_jar(scylla_types
      SOURCES ${JAVA_FILES}
      INCLUDE_JARS ${LCM_JAR_PATH}
      OUTPUT_DIR ${LCM_GEN_JAVA_DIR}
    )
  else()
    message(WARNING "Java or lcm-java not found. JAR will not be built.")
  endif()
endif()
