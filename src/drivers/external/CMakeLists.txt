set(RPLIDAR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rplidar_sdk)

add_library(rplidar_sdk STATIC
  ${RPLIDAR_DIR}/sdk/src/rplidar_driver.cpp
  ${RPLIDAR_DIR}/sdk/src/hal/thread.cpp
  ${RPLIDAR_DIR}/sdk/src/arch/linux/net_serial.cpp
  ${RPLIDAR_DIR}/sdk/src/arch/linux/net_socket.cpp
  ${RPLIDAR_DIR}/sdk/src/arch/linux/timer.cpp
)

target_include_directories(rplidar_sdk PUBLIC
  ${RPLIDAR_DIR}/sdk/include
  ${RPLIDAR_DIR}/sdk/src
)
