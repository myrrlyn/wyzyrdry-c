cmake_minimum_required(VERSION 3.7)
project(wyzyrdry)

set(CMAKE_C_STANDARD 99)

include_directories(include/)
set(LIBRARY_OUTPUT_PATH cmake-build-debug)
set(SOURCE_FILES
        src/wyzyrdry.c
        include/wyzyrdry.h
        src/vec.c
        include/vec.h
        src/slice.c
        include/slice.h
    )
add_library(wyzyrdry ${SOURCE_FILES})