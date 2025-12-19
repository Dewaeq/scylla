file(GLOB SOURCES src/*.cpp)

add_library(quadtree ${SOURCES})

target_include_directories(quadtree PUBLIC include)
