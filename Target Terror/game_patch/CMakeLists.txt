cmake_minimum_required(VERSION 3.6)
project(tt_emu)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -shared -w -ldl -lpthread -m32 -D_GNU_SOURCE")

set(SOURCE_FILES main.c rdtsc.h rtfs.h global.h target.c target.h global.c io.h io.c gfx.h gfx.c rtfs.c rtfs_util.h rtfs_util.c)
add_executable(tt_emu.so ${SOURCE_FILES})