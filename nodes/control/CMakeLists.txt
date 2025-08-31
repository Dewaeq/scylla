add_executable(control main.cpp)
target_link_libraries(control PRIVATE wiringPi ${LCM_LIBRARIES})
