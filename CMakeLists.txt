cmake_minimum_required(VERSION 3.8)
project(wyzyrdry)

set(CMAKE_C_STANDARD 99)

include_directories(include/)
set(SOURCE_FILES src/lib.c include/lib.h src/vec.c include/vec.h)
add_library(wyzyrdry ${SOURCE_FILES})