cmake_minimum_required(VERSION 3.7)
project(wyzyrdry)

set(CMAKE_C_STANDARD 99)

include_directories(include/)
set(LIBRARY_OUTPUT_PATH cmake-build-debug)
set(SOURCE_FILES
		src/wyzyrdry.c
		include/wyzyrdry.h
		src/vec.c
		include/wyzyrdry/vec.h
		src/slice.c
		include/wyzyrdry/slice.h
		src/str.c
		include/wyzyrdry/str.h
		include/wyzyrdry/enum.h
		src/ringbuf.c
		include/wyzyrdry/ringbuf.h
	)
add_library(wyzyrdry ${SOURCE_FILES})
