cmake_minimum_required(VERSION 3.10)
project(pm_emu C)

set(CMAKE_C_STANDARD 11)
set ( CMAKE_C_FLAGS "-m32")
add_library(pm_emu SHARED library.c library.h patches.c patches.h card/card_emu.c card/card_emu.h)
set_target_properties(pm_emu PROPERTIES PREFIX "")
set_target_properties(pm_emu PROPERTIES SUFFIX ".so")