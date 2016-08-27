cmake_minimum_required(VERSION 3.6)

set(CROSSLIB_PORT "linux")
include(../crosslib.cmake)

set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lgtest")

add_executable(test1 test1.cpp ${COMMON_SOURCES})
