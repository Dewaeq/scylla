file(GLOB SOURCES src/*.cpp)

add_executable(lidar ${SOURCES})
target_include_directories(lidar PRIVATE include/)
target_link_libraries(lidar PRIVATE rplidar_sdk ${LCM_LIBRARIES} Eigen3::Eigen grid)
