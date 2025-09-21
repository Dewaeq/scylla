file(GLOB SOURCES src/*.cpp)

add_library(icp ${SOURCES})

target_include_directories(icp PUBLIC include)
target_link_libraries(icp KDTree)
