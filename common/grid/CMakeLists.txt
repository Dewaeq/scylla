file(GLOB SOURCES ./src/probability_grid.cpp)

add_library(grid ${SOURCES})

target_include_directories(grid PUBLIC include)
