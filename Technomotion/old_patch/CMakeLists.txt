cmake_minimum_required(VERSION 3.14)
project(technotools C)
set(CMAKE_C_STANDARD 90)

add_library(technotools SHARED library.c cmos_rtc.c cmos_rtc.h global.h technomotion_io.c technomotion_io.h key_bindings.h global.c)
set_target_properties(technotools PROPERTIES PREFIX "")
set_target_properties(technotools PROPERTIES OUTPUT_NAME "tiopatch")
set(CMAKE_C_FLAGS -Wl,--kill-at)
set_target_properties(technotools
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

