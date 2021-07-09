cmake_minimum_required(VERSION 3.14)
project(COB_EMU)
set(CMAKE_CXX_STANDARD 17)
set ( CMAKE_CXX_FLAGS "-static -w")
add_library(COB_EMU SHARED cob_emu.cpp IOPortEmu/Base.cpp IOPortEmu/Base.h IOPortEmu/IOPortHandler.cpp IOPortEmu/IOPortHandler.h IOPortEmu/Linux.cpp IOPortEmu/Linux.h IOPortEmu/Win32.cpp IOPortEmu/Win32.h dbg_utils.cpp dbg_utils.h EXPotatoIO.cpp EXPotatoIO.h KeyboardIO.cpp KeyboardIO.h)

set_target_properties(COB_EMU PROPERTIES PREFIX "")
set_target_properties(COB_EMU PROPERTIES OUTPUT_NAME "cob_emu")

set_target_properties(COB_EMU
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

