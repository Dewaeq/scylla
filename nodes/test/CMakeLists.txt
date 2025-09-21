file(GLOB SOURCES src/*.cpp)

add_executable(test ${SOURCES})
target_include_directories(test PRIVATE include)
target_link_libraries(test PRIVATE ${LCM_LIBRARIES} Eigen3::Eigen Ceres::ceres grid)
