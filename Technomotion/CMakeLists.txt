cmake_minimum_required(VERSION 3.16)
project(technotools C)

set(CMAKE_C_STANDARD 99)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/../bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "D:\\techmo\\")
set(CMAKE_SHARED_LIBRARY_PREFIX  "")

add_library(technotools SHARED kitchen_sink/kitchen_sink.h kitchen_sink/target.h kitchen_sink/ks.debug.c kitchen_sink/ks.memutils.h kitchen_sink/ks.memutils.c kitchen_sink/ks.hotpatch.c fs.codepage.fix.c gfx.c fs.redirect.c io.cmos_rtc.c io.technoinput.c technotools.c io.technooutput.c io.technooutput.h io.technoio.c)

set_target_properties(technotools PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
set_target_properties(technotools PROPERTIES OUTPUT_NAME "technotools")

project(technoloader C)
add_executable(technoloader kitchen_sink/ks.misc.c technoloader.c)
set_target_properties(technoloader PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
