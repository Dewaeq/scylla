add_executable(lidar main.cpp)
target_link_libraries(lidar PRIVATE rplidar_sdk ${LCM_LIBRARIES})
