file(GLOB SOURCES src/*.cpp)

add_executable(slam_frontend ${SOURCES})

target_include_directories(slam_frontend PRIVATE include)
target_link_libraries(slam_frontend PRIVATE ${LCM_LIBRARIES} Eigen3::Eigen grid icp)
