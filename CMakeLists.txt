cmake_minimum_required(VERSION 2.6)
project(cob-a1 CXX)

IF(NOT MSVC)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-std=c++11)
    ADD_DEFINITIONS(-g)
ENDIF()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)
