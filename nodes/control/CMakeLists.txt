file(GLOB SOURCES src/*.cpp)

add_executable(control ${SOURCES})
target_link_libraries(control PRIVATE wiringPi ${LCM_LIBRARIES})
