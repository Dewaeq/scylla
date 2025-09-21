add_subdirectory(lidar)
add_subdirectory(test)
add_subdirectory(slam_frontend)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  add_subdirectory(control)
else()
  message(STATUS "skipping control node (not building for rpi)")
endif()

