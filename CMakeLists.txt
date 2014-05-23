cmake_minimum_required(VERSION 2.6)
project(cob-a1 CXX)

IF(NOT MSVC)
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-std=c++11)
    ADD_DEFINITIONS(-g)
ENDIF()

set(LLVM_INC_DIR "inc" CACHE PATH "bla")
set(LLVM_BIN_DIR "bin" CACHE PATH "bla2")

include_directories(${CMAKE_SOURCE_DIR}/include ${LLVM_INC_DIR})
add_subdirectory(src)
