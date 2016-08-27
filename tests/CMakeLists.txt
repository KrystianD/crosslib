cmake_minimum_required(VERSION 3.6)

set(CROSSLIB_PORT "linux")
include(../crosslib.cmake)

set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lgtest")

add_executable(test_thread test_thread.cpp ${COMMON_SOURCES})
add_executable(test_mutex test_mutex.cpp ${COMMON_SOURCES})
add_executable(test_condvar test_condvar.cpp ${COMMON_SOURCES})
add_executable(test_utils test_utils.cpp ${COMMON_SOURCES})
