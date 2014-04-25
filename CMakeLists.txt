cmake_minimum_required(VERSION 2.6)
project(cob-a1 CXX)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
ENDIF()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)