cmake_minimum_required(VERSION 2.6)
project(cob-a1 CXX)

IF(NOT MSVC)
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-std=c++11)
    add_definitions(-g)
	add_definitions(-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
else()
	add_definitions(-D_VARIADIC_MAX=10 -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS -wd4146 -wd4180 -wd4244 -wd4267 -wd4345 -wd4351 -wd4355 -wd4503 -wd4624 -wd4800 -wd4291 -w14062 -we4238 -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
endif()

set(LLVM_INC_DIR "/usr/include/llvm-3.4/;/usr/include/llvm-c-3.4" CACHE PATH "bla")
set(LLVM_LIB_DIR "/usr/lib/llvm-3.4/lib" CACHE PATH "bla2")

include_directories(${CMAKE_SOURCE_DIR}/include ${LLVM_INC_DIR})
link_directories(${LLVM_LIB_DIR})
add_subdirectory(src)
